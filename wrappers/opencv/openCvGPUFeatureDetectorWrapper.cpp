#include "openCvGPUFeatureDetectorWrapper.h"
#include "openCvKeyPointsWrapper.h"

#include "openCvDefaultParams.h"

#include "core/utils/global.h"

static bool deviceFound = false;
static bool cudaDevice = false;

#include <stdio.h>

#ifdef WITH_OPENCV_3x
#   include <opencv2/cudafeatures2d.hpp>
#   include <opencv2/core/ocl.hpp>
#   include <opencv2/xfeatures2d.hpp>
#   include <opencv2/xfeatures2d/cuda.hpp>

using namespace cv::cuda;

#else
#   include <opencv2/features2d/features2d.hpp>    // cv::FeatureDetector
#   include <opencv2/nonfree/gpu.hpp>       // cv::gpu::SURF
#   include <opencv2/nonfree/ocl.hpp>       // cv::ocl::SURF
#   include <opencv2/gpu/gpu.hpp>           // cv::gpu::ORB
// #include <opencv2/ocl/ocl.hpp>        // cv::ocl::ORB - not implemented

using namespace cv::gpu;

#endif

using namespace std;
using namespace cv::ocl;

#ifdef WITH_OPENCV_3x

struct SmartPtrHolder
{
    SmartPtrHolder() : tag(SIFT), sift() {}
    ~SmartPtrHolder() {}
    enum {
        ORB, SURF
    } tag;

    union {
        cv::Ptr< cv::cuda::ORB >          orb;
        cv::Ptr< cv::xfeatures2d::SURF >  surf;
    };

    cv::DescriptorExtractor *get() {
        switch (tag) {
        case ORB:
            return orb.get();
        case SURF:
            return surf.get();
        default:
            return nullptr;
        }
    }

    void set(cv::Ptr<cv::cuda::ORB> value) {
        tag = ORB;
        orb = value;
    }
    void set(cv::Ptr<cv::xfeatures2d::SURF> value) {
        tag = SURF;
        surf = value;
    }
};

OpenCvGPUFeatureDetectorWrapper::OpenCvGPUFeatureDetectorWrapper(cv::cuda::SURF_CUDA *detector) : detectorSURF_CUDA(detector),
    holder(0)
{}

OpenCvGPUFeatureDetectorWrapper::OpenCvGPUFeatureDetectorWrapper(SmartPtrHolder *holder) : holder(holder),
    detectorSURF_CUDA(0)
{}

#else

OpenCvGPUFeatureDetectorWrapper::OpenCvGPUFeatureDetectorWrapper(SURF_GPU *detector) : detectorSURF_CUDA(detector),
    detectorORB_CUDA(0),
    detectorSURF_OCL(0)
{}

OpenCvGPUFeatureDetectorWrapper::OpenCvGPUFeatureDetectorWrapper(ORB_GPU *detector) : detectorORB_CUDA(detector),
    detectorSURF_CUDA(0),
    detectorSURF_OCL(0)
{}

OpenCvGPUFeatureDetectorWrapper::OpenCvGPUFeatureDetectorWrapper(SURF_OCL *detector) : detectorSURF_OCL(detector),
    detectorSURF_CUDA(0),
    detectorORB_CUDA(0)
{}

#endif

OpenCvGPUFeatureDetectorWrapper::~OpenCvGPUFeatureDetectorWrapper()
{
#ifdef WITH_OPENCV_3x
    delete holder;
#else 
    delete detectorORB_CUDA;
    delete detectorSURF_OCL;
#endif

    delete detectorSURF_CUDA;
}

double OpenCvGPUFeatureDetectorWrapper::getProperty(const std::string &name) const
{
    CORE_UNUSED(name);
    return 0.0;
}

void OpenCvGPUFeatureDetectorWrapper::setProperty(const std::string &name, const double &value)
{
    CORE_UNUSED(name);
    CORE_UNUSED(value);
}

struct OpenCLRemapCache
{
    cv::Mat unused0;
    cv::Mat unused1;
#ifdef WITH_OPENCV_3x
	UMat mat0;
	UMat mat1;
#else
	oclMat mat0;
	oclMat mat1;
#endif
};

struct CudaRemapCache
{
    cv::Mat unused0;
    cv::Mat unused1;
	GpuMat mat0;
	GpuMat mat1;
};

void OpenCvGPUFeatureDetectorWrapper::detectImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int K, void* pRemapCache)
{
	if (image.getType() != corecvs::BufferType::U8 || !image.isValid())
    {
        std::cerr << __LINE__ << "Invalid image type" << std::endl;
    }

	std::vector<cv::KeyPoint> kps;
#ifdef WITH_OPENCV_3x
    if (detectorSURF_CUDA)
    {
        cv::cuda::GpuMat img(convert(image));
		if (pRemapCache)
		{
			GpuMat remapped;
			CudaRemapCache* p = (CudaRemapCache*)(pRemapCache);
			cv::cuda::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
			img = remapped;
		}
        cv::cuda::GpuMat mask;
        (*detectorSURF_CUDA)(img, mask, kps);  
    }
    else if (holder)
    {
        auto detector = holder->get();
        if (holder->tag == SmartPtrHolder::SURF) // openCL implementation
        {
			cv::UMat img(convert(image));
			if (pRemapCache)
			{
				UMat remapped;
				OpenCLRemapCache* p = (OpenCLRemapCache*)(pRemapCache);
				cv::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
				img = remapped;
			}
            detector->detect(img, kps);
        }

        if (holder->tag == SmartPtrHolder::ORB) // CUDA implementation
        {
            cv::cuda::GpuMat img(convert(image));
			if (pRemapCache)
			{
				GpuMat remapped;
				CudaRemapCache* p = (CudaRemapCache*)(pRemapCache);
				cv::cuda::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
				img = remapped;
			}
            detector->detect(img, kps);
        }           
    }

#else
    if (detectorSURF_CUDA || detectorORB_CUDA)
    {
        cv::gpu::GpuMat img(convert(image));
		if (pRemapCache)
		{
			GpuMat remapped;
			CudaRemapCache* p = (CudaRemapCache*)(pRemapCache);
			cv::gpu::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
			img = remapped;
		}

        cv::gpu::GpuMat mask;

        if (detectorSURF_CUDA)
        {
            //max keypoints = min(keypointsRatio * img.size().area(), 65535)
            //detectorSURF_CUDA->keypointsRatio = (float)K / img.size().area();
            (*detectorSURF_CUDA)(img, mask, kps);
        }     
        else if (detectorORB_CUDA)
        {
            (*detectorORB_CUDA)(img, mask, kps);
        }

    }
    else if (detectorSURF_OCL)
    {
        cv::ocl::oclMat img(convert(image));
		if (pRemapCache)
		{
			oclMat remapped;
			OpenCLRemapCache* p = (OpenCLRemapCache*)(pRemapCache);
			cv::ocl::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST, cv::BORDER_CONSTANT);
			img = remapped;
		}

        cv::ocl::oclMat mask;
        //max keypoints = min(keypointsRatio * img.size().area(), 65535)
        //detectorSURF_OCL->keypointsRatio = (float)K / img.size().area();
        (*detectorSURF_OCL)(img, mask, kps);
    }
#endif

	keyPoints.clear();
    FOREACH(const cv::KeyPoint &kp, kps)
    {
		keyPoints.push_back(convert(kp));
    }

	std::sort(keyPoints.begin(), keyPoints.end(), [](const KeyPoint &l, const KeyPoint &r) { return l.response > r.response; });
	keyPoints.resize(std::min((int)keyPoints.size(), K));
}

bool FindGPUDevice(bool& cudaApi)
{
    if (deviceFound)
    {
        cudaApi = cudaDevice;
        return true;
    }

	int numCudaDevices = 0;
	try 
	{
		numCudaDevices = getCudaEnabledDeviceCount();
	}
	catch (cv::Exception)
	{
		numCudaDevices = 0;
	}
		
    bool initProvider = false;
    for (int idx = 0; idx < numCudaDevices; idx++)
    {
        setDevice(idx);
#ifdef WITH_OPENCV_3x
        DeviceInfo devInfo(idx);
#else
        cv::gpu::DeviceInfo devInfo(idx);     
#endif
        if (devInfo.isCompatible())
        {
            deviceFound = initProvider = true; // found cuda device to use
            cout << "OpeCV uses CUDA "<< devInfo.name() << endl;
            cudaDevice = cudaApi = true;
            break;
        }

        resetDevice();
    }

    //initProvider = false;
    if (!initProvider) // if no cuda device found, try use first found opencl gpus
    {
#ifdef WITH_OPENCV_3x
        if (!cv::ocl::haveOpenCL())
        {
            cout << "OpenCL is not available..." << endl;
            return false;
        }

        cv::ocl::Context context;
        if (!context.create(cv::ocl::Device::TYPE_GPU))
        {
            cout << "Failed creating the context..." << endl;
            //return;
        }

        cout << context.ndevices() << " GPU devices are detected." << endl; //This bit provides an overview of the OpenCL devices you have in your computer
        for (int i = 0; i < context.ndevices(); i++)
        {
            cv::ocl::Device device = context.device(i);
            cout << "name:              " << device.name() << endl;
            cout << "available:         " << device.available() << endl;
            cout << "imageSupport:      " << device.imageSupport() << endl;
            cout << "OpenCL_C_Version:  " << device.OpenCL_C_Version() << endl;
            cout << endl;
        }

        cv::ocl::Device(context.device(0)); //Here is where you change which GPU to use (e.g. 0 or 1)
        deviceFound = initProvider = true;

        cv::ocl::setUseOpenCL(true);
        cudaDevice = cudaApi = false;
#else
        DevicesInfo oclDevices;
        getOpenCLDevices(oclDevices, cv::ocl::CVCL_DEVICE_TYPE_GPU);
        if (oclDevices.size())
        {
			for (uint i = 0; i < oclDevices.size(); i++)
			{
				const cv::ocl::DeviceInfo* device = oclDevices[i];
				if (device->deviceVersionMajor > 1 || device->deviceVersionMinor >= 2) // check opencl device support 1.2
				{
					cv::ocl::setDevice(device);
					deviceFound = initProvider = true;
					cudaDevice = cudaApi = false;
					cout << "OpeCV uses OpenCL " << device->deviceName << endl;
					break;
				}
			}                 
        }
#endif
    }

    return initProvider;
}

void init_opencv_gpu_detectors_provider()
{
    bool cudaApi = false;
    if (FindGPUDevice(cudaApi))
        FeatureDetectorProvider::getInstance().add(new OpenCvGPUFeatureDetectorProvider(cudaApi));
}

OpenCvGPUFeatureDetectorProvider::OpenCvGPUFeatureDetectorProvider(bool _cudaApi) : cudaApi(_cudaApi) {}

#define SWITCH_TYPE(str, expr) \
	if(type == #str) \
	{ \
		expr; \
	}

FeatureDetector* OpenCvGPUFeatureDetectorProvider::getFeatureDetector(const DetectorType &type, const std::string &params)
{
	SurfParams surfParams(params);
	OrbParams orbParams(params);

#ifdef WITH_OPENCV_3x
    SmartPtrHolder* holder = new SmartPtrHolder;
    if (type == "SURF_GPU")
    {
        if (cudaApi)
        {
            delete holder;
            holder = 0;
            return new OpenCvGPUFeatureDetectorWrapper(new cv::cuda::SURF_CUDA(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.01f, surfParams.upright));
        }
        else
        {
            cv::Ptr< cv::xfeatures2d::SURF > ptr = cv::xfeatures2d::SURF::create(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, surfParams.upright);
            holder->set(ptr);
            return new OpenCvGPUFeatureDetectorWrapper(holder);
        }
    }

    if (type == "ORB_GPU")
    {
        if (cudaApi)
        {
            cv::Ptr< cv::cuda::ORB > ptr = cv::cuda::ORB::create(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize);
            holder->set(ptr);
            return new OpenCvGPUFeatureDetectorWrapper(holder);
        }
    }

#else
    if (cudaApi)
    {
        SWITCH_TYPE(SURF_GPU,
            return new OpenCvGPUFeatureDetectorWrapper(new cv::gpu::SURF_GPU(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.01f, surfParams.upright));)
        SWITCH_TYPE(ORB_GPU,
            return new OpenCvGPUFeatureDetectorWrapper(new cv::gpu::ORB_GPU(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize));)
    }
    else
        SWITCH_TYPE(SURF_GPU,
            return new OpenCvGPUFeatureDetectorWrapper(new cv::ocl::SURF_OCL(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, 0.01f, surfParams.upright));)
    
#endif 

    return 0;
}

bool OpenCvGPUFeatureDetectorProvider::provides(const DetectorType &type)
{
    if (cudaApi)
    {
        SWITCH_TYPE(SURF_GPU, return true;);
        SWITCH_TYPE(ORB_GPU, return true;);
    }
    else
        SWITCH_TYPE(SURF_GPU, return true;);
    
    return false;
}

std::vector<string> OpenCvGPUFeatureDetectorProvider::provideHints()
{
    std::vector<std::string> toReturn;
    if (cudaApi)
    {
        toReturn.push_back("SURF_GPU");
        toReturn.push_back("ORB_GPU");
    } else {
        toReturn.push_back("SURF_GPU");
    }
    return toReturn;
}

#undef SWITCH_TYPE
