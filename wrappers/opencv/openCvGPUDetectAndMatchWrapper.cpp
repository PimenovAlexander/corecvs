#include "openCvGPUDetectAndMatchWrapper.h"
#include "openCvKeyPointsWrapper.h"
#include "core/features2d/bufferReaderProvider.h"
#include "core/features2d/featureMatchingPipeline.h"
#include "openCvDefaultParams.h"

#ifdef WITH_OPENCV_GPU

#ifndef WITH_OPENCV_3x

#include <opencv2/nonfree/gpu.hpp>       // cv::gpu::SURF
#include <opencv2/nonfree/ocl.hpp>       // cv::ocl::SURF
#include <opencv2/gpu/gpu.hpp>           // cv::gpu::ORB
// #include <opencv2/ocl/ocl.hpp>        // cv::ocl::ORB - not implemented

using namespace cv::gpu;
using namespace cv::ocl;

extern void makeMatchingPlan(FeatureMatchingPipeline& pipeline);

struct GPUImageDetectData
{
	GpuMat cudaImg;
	GpuMat cudaKeypoints;
	GpuMat cudaDescriptors;
    oclMat oclImg;
    oclMat oclKeypoints;
    oclMat oclDescriptors;

	GPUImageDetectData(const GpuMat& img) : cudaImg(img) {}
    GPUImageDetectData(const oclMat& img) : oclImg(img) {}
};

void OpenCvGPUDetectExtractAndMatchWrapper::detectExtractAndMatchImpl(FeatureMatchingPipeline& pipeline, int nMaxKeypoints, int numResponcesPerPoint)
{
	const size_t numImages = pipeline.images.size();
	std::vector<GPUImageDetectData> gpuImages;

	for (size_t i = 0; i < numImages; i++)
	{
		Image& image = pipeline.images[i];
		image.keyPoints.keyPoints.clear();

		std::unique_ptr<BufferReader> reader(BufferReaderProvider::getInstance().getBufferReader(image.filename));
		corecvs::RuntimeTypeBuffer img = reader->read(image.filename);
        
        if (detectorSURF_OCL)
        {
            GPUImageDetectData gpuImage(cv::ocl::oclMat(convert(img)));
            gpuImages.push_back(gpuImage);
        }
        else
        {
            GPUImageDetectData gpuImage(cv::gpu::GpuMat(convert(img)));
            gpuImages.push_back(gpuImage);
        }
	}

	// GPU work begins now

    for (size_t i = 0; i < numImages; i++)
    {
        if (detectorSURF_OCL)
        {
            //max keypoints = min(keypointsRatio * img.size().area(), 65535)
            detectorSURF_OCL->keypointsRatio = (float)nMaxKeypoints / gpuImages[i].oclImg.size().area();
            (*detectorSURF_OCL)(gpuImages[i].oclImg, cv::ocl::oclMat(), gpuImages[i].oclKeypoints, gpuImages[i].oclDescriptors, false);
        }      
        else if (detectorSURF_CUDA)
        {
            //max keypoints = min(keypointsRatio * img.size().area(), 65535)
            detectorSURF_CUDA->keypointsRatio = (float)nMaxKeypoints / gpuImages[i].cudaImg.size().area();
            (*detectorSURF_CUDA)(gpuImages[i].cudaImg, cv::gpu::GpuMat(), gpuImages[i].cudaKeypoints, gpuImages[i].cudaDescriptors, false);
        }
        else if (detectorORB_CUDA)
        {
            (*detectorORB_CUDA)(gpuImages[i].cudaImg, cv::gpu::GpuMat(), gpuImages[i].cudaKeypoints, gpuImages[i].cudaDescriptors);
        }
	}

#if 0
	for (size_t i = 0; i < numImages; i++)
	{
		gpuImages[i].cudaImg.~GpuMat();
        gpuImages[i].oclImg.~oclMat();
	}
#endif

	std::vector< std::vector< std::vector<cv::DMatch> > > matchesPerImage;
	matchesPerImage.resize(numImages * numImages); // match all images vs all

	for (size_t i = 0; i < numImages; i++)
	{
		//for (size_t j = i + 1; j < numImages; j++)
		for (size_t j = 0; j < numImages; j++)
		{
			std::vector< std::vector<cv::DMatch> > matches_cv;
            if (i != j)
            {
                if (matcherBF_CUDA)
                    matcherBF_CUDA->knnMatch(gpuImages[i].cudaDescriptors, gpuImages[j].cudaDescriptors, matches_cv, numResponcesPerPoint);
                else if (matcherBF_OCL)
                    matcherBF_OCL->knnMatch(gpuImages[i].oclDescriptors, gpuImages[j].oclDescriptors, matches_cv, numResponcesPerPoint);
            }

			matchesPerImage[i * numImages + j] = matches_cv;
		}
	}

	// GPU work is done by now, readback and convert results

    for (size_t i = 0; i < numImages; i++)
    {
        Image& image = pipeline.images[i];
        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;

        if (detectorSURF_CUDA)
        {
            detectorSURF_CUDA->downloadKeypoints(gpuImages[i].cudaKeypoints, keypoints);	// copy GPU -> CPU
            gpuImages[i].cudaDescriptors.download(descriptors); // copy GPU -> CPU
        }
        else if (detectorORB_CUDA)
        {
            cv::Mat keypointsMat;
            gpuImages[i].cudaDescriptors.download(descriptors); // copy GPU -> CPU
            gpuImages[i].cudaKeypoints.download(keypointsMat); // copy GPU -> CPU
            detectorORB_CUDA->convertKeyPoints(keypointsMat, keypoints);
        }
		else if (detectorSURF_OCL)
		{
			detectorSURF_OCL->downloadKeypoints(gpuImages[i].oclKeypoints, keypoints);	// copy GPU -> CPU
			gpuImages[i].oclDescriptors.download(descriptors); // copy GPU -> CPU
		}
	
        image.keyPoints.keyPoints.clear();
		FOREACH(const cv::KeyPoint &kp, keypoints)
		{
			image.keyPoints.keyPoints.push_back(convert(kp));
		}

		image.descriptors.mat = convert(descriptors);
	}

    makeMatchingPlan(pipeline); // compatibility call, to fill in required data

	size_t S = pipeline.matchPlan.plan.size();
    CORE_ASSERT_TRUE_S(S);

	MatchPlan &matchPlan = pipeline.matchPlan;
	RawMatches &rawMatches = pipeline.rawMatches;

    rawMatches.matches.clear();
    rawMatches.matches.resize(matchPlan.plan.size());

	std::vector<std::vector<RawMatch>> matches;
	for (size_t i = 0; i < S; i++)
	{
		size_t s = i;
		size_t I = matchPlan.plan[s].queryImg;
		size_t J = matchPlan.plan[s].trainImg;
		auto &query = matchPlan.plan[s];
		
		std::vector< std::vector<cv::DMatch> >& matches_cv = matchesPerImage[I * numImages + J];
		matches.clear();
		matches.resize(matches_cv.size());

		for (size_t idx = 0; idx < matches.size(); ++idx)
		{
			for (std::vector<cv::DMatch>::iterator m = matches_cv[idx].begin(); m != matches_cv[idx].end(); ++m)
			{
				matches[idx].push_back(convert(*m));
			}
		}

		for (std::vector<std::vector<RawMatch> >::iterator v = matches.begin(); v != matches.end(); ++v)
		{
			std::array<RawMatch, 2> mk;
			for (size_t i = 0; i < v->size(); ++i)
			{
				mk[i] = (*v)[i];
			}
			rawMatches.matches[s].push_back(mk);
		}

		// It's time to replace indicies
		for (std::deque<std::array<RawMatch, 2> >::iterator v = rawMatches.matches[s].begin(); v != rawMatches.matches[s].end(); ++v)
		{
			for (std::array<RawMatch, 2>::iterator m = v->begin(); m != v->end() && m->isValid(); ++m)
			{
				m->featureQ = query.queryFeatures[m->featureQ];
				m->featureT = query.trainFeatures[m->featureT];
			}
		}
	}
}

OpenCvGPUDetectExtractAndMatchWrapper::OpenCvGPUDetectExtractAndMatchWrapper(SURF_GPU *detector, cv::gpu::BruteForceMatcher_GPU_base* matcher)
    : detectorSURF_CUDA(detector)
    , detectorORB_CUDA(0)
    , detectorSURF_OCL(0)
    , matcherBF_CUDA(matcher)
    , matcherBF_OCL(0)
{}

OpenCvGPUDetectExtractAndMatchWrapper::OpenCvGPUDetectExtractAndMatchWrapper(ORB_GPU *detector, cv::gpu::BruteForceMatcher_GPU_base* matcher)
    : detectorORB_CUDA(detector)
    , detectorSURF_CUDA(0)
    , detectorSURF_OCL(0)
    , matcherBF_CUDA(matcher)
    , matcherBF_OCL(0)
{}

OpenCvGPUDetectExtractAndMatchWrapper::OpenCvGPUDetectExtractAndMatchWrapper(SURF_OCL *detector, cv::ocl::BruteForceMatcher_OCL_base* matcher)
    : detectorSURF_OCL(detector)
    , detectorSURF_CUDA(0)
    , detectorORB_CUDA(0)
    , matcherBF_CUDA(0)
    , matcherBF_OCL(matcher)
{}

OpenCvGPUDetectExtractAndMatchWrapper::~OpenCvGPUDetectExtractAndMatchWrapper()
{
    delete detectorORB_CUDA;
    delete detectorSURF_OCL;
    delete detectorSURF_CUDA;
    delete matcherBF_CUDA;
    delete matcherBF_OCL;
}

extern bool FindGPUDevice(bool& cudaApi);

bool init_opencv_gpu_detect_extract_and_match_provider(bool& cudaApi)
{
	cudaApi = false;
    if (FindGPUDevice(cudaApi))
    {
        DetectExtractAndMatchProvider::getInstance().add(new OpenCvGPUDetectExtractAndMatchProvider(cudaApi));
        return true;
    }
		
    return false;
}

OpenCvGPUDetectExtractAndMatchProvider::OpenCvGPUDetectExtractAndMatchProvider(bool _cudaApi) : cudaApi(_cudaApi) {}

#define SWITCH_TYPE(str, expr) \
	if(type == #str) \
		{ \
		expr; \
		}

DetectExtractAndMatch* OpenCvGPUDetectExtractAndMatchProvider::getDetector(const DetectorType &detectorType, const DescriptorType &descriptorType, const MatcherType &matcherType, const std::string &params)
{
	SurfParams surfParams(params);

    if (cudaApi)
    {
        if (detectorType == "SURF_GPU" && descriptorType == "SURF_GPU" && matcherType == "BF_GPU")
            return new OpenCvGPUDetectExtractAndMatchWrapper(
                new cv::gpu::SURF_GPU(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.01f, surfParams.upright),
                new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::L2Dist)
            );

        if (detectorType == "ORB_GPU" && descriptorType == "ORB_GPU" && matcherType == "BF_GPU")
        {
            OrbParams orbParams(params);
            return new OpenCvGPUDetectExtractAndMatchWrapper(
                new cv::gpu::ORB_GPU(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize),
                new cv::gpu::BruteForceMatcher_GPU_base(cv::gpu::BruteForceMatcher_GPU_base::HammingDist)
            );
        }
    }
    else if (detectorType == "SURF_GPU" && descriptorType == "SURF_GPU" && matcherType == "BF_GPU")
        return new OpenCvGPUDetectExtractAndMatchWrapper(
            new cv::ocl::SURF_OCL(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.01f, surfParams.upright),
            new cv::ocl::BruteForceMatcher_OCL_base(cv::ocl::BruteForceMatcher_OCL_base::L2Dist)
        );
	return 0;
}

bool OpenCvGPUDetectExtractAndMatchProvider::provides(const DetectorType &detectorType, const DescriptorType &descriptorType, const MatcherType &matcherType)
{
    if (cudaApi && detectorType == "ORB_GPU" && descriptorType == "ORB_GPU" && matcherType == "BF_GPU")
        return true;

    if (detectorType == "SURF_GPU" && descriptorType == "SURF_GPU" && matcherType == "BF_GPU")
        return true;

	return false;
}

#else // !WITH_OPENCV_3x

bool  init_opencv_gpu_detect_extract_and_match_provider(bool& cudaApi)
{
	cudaApi = false;
	return false;
}

#endif // WITH_OPENCV_3x

#endif // WITH_OPENCV_GPU
