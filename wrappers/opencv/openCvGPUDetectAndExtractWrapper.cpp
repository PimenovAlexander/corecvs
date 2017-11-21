#include "openCvGPUDetectAndExtractWrapper.h"
#include "openCvKeyPointsWrapper.h"
#include "core/utils/global.h"
#include "core/features2d/bufferReaderProvider.h"
#include "core/features2d/featureMatchingPipeline.h"
#include "openCvDefaultParams.h"

#ifdef WITH_OPENCV_GPU

#ifndef WITH_OPENCV_3x

#include <opencv2/nonfree/features2d.hpp> // cv::SURF
#include <opencv2/nonfree/gpu.hpp>        // cv::gpu::SURF
#include <opencv2/nonfree/ocl.hpp>        // cv::ocl::SURF
#include <opencv2/gpu/gpu.hpp>            // cv::gpu::ORB
// #include <opencv2/ocl/ocl.hpp>         // cv::ocl::ORB - not implemented

using namespace cv::gpu;
using namespace cv::ocl;
using namespace cv;

struct OpenCLRemapCache
{
    Mat unused0;
    Mat unused1;
	oclMat mat0;
	oclMat mat1;
};

struct CudaRemapCache
{
    Mat unused0;
    Mat unused1;
	GpuMat mat0;
	GpuMat mat1;
};

void OpenCvGPUDetectAndExtractWrapper::detectAndExtractImpl(corecvs::RuntimeTypeBuffer &image, std::vector<::KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descriptors, int nMaxKeypoints, void* pRemapCache)
{
	if (image.getType() != corecvs::BufferType::U8 || !image.isValid())
	{
		std::cerr << __LINE__ << "Invalid image type" << std::endl;
	}

	std::vector<cv::KeyPoint> kps;
	cv::Mat cv_descriptors;
	if (detectorSURF_CUDA || detectorORB_CUDA)
	{
		GpuMat img(convert(image));
		if (pRemapCache)
		{
			GpuMat remapped;
			CudaRemapCache* p = (CudaRemapCache*)(pRemapCache);
			cv::gpu::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
			img = remapped;
		}

		GpuMat cudaDescriptors;
		GpuMat mask;
		if (detectorSURF_CUDA)
		{
			//max keypoints = min(keypointsRatio * img.size().area(), 65535)
			//detectorSURF_CUDA->keypointsRatio = ( float )nMaxKeypoints / img.size().area();
			(*detectorSURF_CUDA)(img, mask, kps, cudaDescriptors, false);
		}
		else if (detectorORB_CUDA)
		{
			(*detectorORB_CUDA)(img, mask, kps, cudaDescriptors);
		}

		cudaDescriptors.download(cv_descriptors); // copy GPU -> CPU

	}
	else if (detectorSURF_OCL)
	{
		oclMat img(convert(image));
		if (pRemapCache)
		{
			oclMat remapped;
			OpenCLRemapCache* p = (OpenCLRemapCache*)(pRemapCache);
			cv::ocl::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST, cv::BORDER_CONSTANT);
			img = remapped;
		}

		oclMat mask;
		oclMat oclDescriptors;
		//max keypoints = min(keypointsRatio * img.size().area(), 65535)
		//detectorSURF_OCL->keypointsRatio = ( float )nMaxKeypoints / img.size().area();
		(*detectorSURF_OCL)(img, mask, kps, oclDescriptors, false);
		oclDescriptors.download(cv_descriptors); // copy GPU -> CPU
	}
	else if (detectorSURF)
	{
		Mat img(convert(image));
		(*detectorSURF)(img, Mat(), kps, cv_descriptors, false);
	}
	else if (detectorORB)
	{
		Mat img(convert(image));
		(*detectorORB)(img, Mat(), kps, cv_descriptors, false);
	}

	keyPoints.clear();
	std::vector < std::pair< int, float > > sortingData;
	sortingData.resize(kps.size());

	for (int i = 0; i < kps.size(); i++)
		sortingData[i] = std::pair< int, float >(i, kps[i].response);

	std::sort(sortingData.begin(), sortingData.end(), [](const std::pair< int, float > &l, const std::pair< int, float > &r) { return l.second > r.second; });

	const int newCount = std::min((int)sortingData.size(), nMaxKeypoints);
	Mat sortedDescriptors(newCount, cv_descriptors.cols, cv_descriptors.type());
	//const size_t szElement = sortedDescriptors.elemSize1();
	const size_t szElements = sortedDescriptors.elemSize1() * cv_descriptors.cols;
	
	for (int i = 0; i < newCount; i++)
	{
		int idx = sortingData[i].first;
		const cv::KeyPoint &kp = kps[idx];
		keyPoints.push_back(convert(kp));
		//for (int j = 0; j < cv_descriptors.cols; j++)
		//	memcpy(sortedDescriptors.ptr(i, j), cv_descriptors.ptr(idx, j), szElement);
		memcpy(sortedDescriptors.ptr(i, 0), cv_descriptors.ptr(idx, 0), szElements);
	}

	descriptors = convert(sortedDescriptors);
}  

void OpenCvGPUDetectAndExtractWrapper::setProperty(const std::string &name, const double &value)
{
	CORE_UNUSED(name);
	CORE_UNUSED(value);
}

double OpenCvGPUDetectAndExtractWrapper::getProperty(const std::string &name) const
{
	CORE_UNUSED(name);
	return 0;
}


OpenCvGPUDetectAndExtractWrapper::OpenCvGPUDetectAndExtractWrapper(cv::ORB  *detectorORB, cv::SURF *detectorSURF, cv::gpu::SURF_GPU *detectorSURF_CUDA, cv::gpu::ORB_GPU *detectorORB_CUDA, cv::ocl::SURF_OCL* detectorSURF_OCL)
    : detectorORB(detectorORB)
    , detectorSURF(detectorSURF)
    , detectorSURF_CUDA(detectorSURF_CUDA)
    , detectorORB_CUDA(detectorORB_CUDA)
    , detectorSURF_OCL(detectorSURF_OCL)
{}

OpenCvGPUDetectAndExtractWrapper::~OpenCvGPUDetectAndExtractWrapper()
{
	delete detectorORB;
	delete detectorSURF;
    delete detectorORB_CUDA;
    delete detectorSURF_OCL;
    delete detectorSURF_CUDA;
}

extern bool FindGPUDevice(bool& cudaApi);

bool init_opencv_gpu_detect_and_extract_provider(bool& cudaApi)
{
	cudaApi = false;
    if (FindGPUDevice(cudaApi))
    {
        DetectAndExtractProvider::getInstance().add(new OpenCvGPUDetectAndExtractProvider(cudaApi));
        return true;
    }
		
    return false;
}

OpenCvGPUDetectAndExtractProvider::OpenCvGPUDetectAndExtractProvider(bool cudaApi) : cudaApi(cudaApi) {}

#define SWITCH_TYPE(str, expr) \
	if (type == #str) \
		{ \
		expr; \
		}

DetectAndExtract* OpenCvGPUDetectAndExtractProvider::getDetector( const DetectorType &detectorType, const DescriptorType &descriptorType, const std::string &params )
{
	SurfParams surfParams(params);
	OrbParams orbParams(params);

    if (cudaApi)
    {
		if (detectorType == "SURF" && descriptorType == "SURF")
			return new OpenCvGPUDetectAndExtractWrapper( 0, new cv::SURF(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, surfParams.upright));

		if (detectorType == "ORB" && descriptorType == "ORB")
			return new OpenCvGPUDetectAndExtractWrapper(new cv::ORB(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize));

        if ( detectorType == "SURF_GPU" && descriptorType == "SURF_GPU" )
            return new OpenCvGPUDetectAndExtractWrapper( 0, 0, new cv::gpu::SURF_GPU( surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.1f, surfParams.upright ) );

        if ( detectorType == "ORB_GPU" && descriptorType == "ORB_GPU" )
            return new OpenCvGPUDetectAndExtractWrapper( 0, 0, 0, new cv::gpu::ORB_GPU( orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize ) );
    }
    else if ( detectorType == "SURF_GPU" && descriptorType == "SURF_GPU" )
		return new OpenCvGPUDetectAndExtractWrapper(0, 0, 0, 0, new cv::ocl::SURF_OCL(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.1f, surfParams.upright));
	
    return 0;
}

bool OpenCvGPUDetectAndExtractProvider::provides(const DetectorType &detectorType, const DescriptorType &descriptorType)
{
    if (cudaApi && detectorType == "ORB_GPU" && descriptorType == "ORB_GPU")
        return true;

    if (detectorType == "SURF_GPU" && descriptorType == "SURF_GPU")
        return true;

	if (detectorType == "SURF" && descriptorType == "SURF")
		return true;

	if (detectorType == "ORB" && descriptorType == "ORB")
		return true;

	return false;
}

#else // !WITH_OPENCV_3x

bool  init_opencv_gpu_detect_and_extract_provider( bool& cudaApi )
{
	cudaApi = false;
	return false;
}

#endif // WITH_OPENCV_3x

#endif // WITH_OPENCV_GPU
