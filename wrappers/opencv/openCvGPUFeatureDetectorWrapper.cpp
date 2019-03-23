#include "openCvGPUFeatureDetectorWrapper.h"
#include "openCvKeyPointsWrapper.h"

#include "openCvDefaultParams.h"

#include "core/utils/global.h"

static bool deviceFound = false;
static bool cudaDevice = false;

#include <stdio.h>


#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/xfeatures2d/cuda.hpp>

using namespace cv::cuda;

using namespace std;
using namespace cv::ocl;

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

OpenCvGPUFeatureDetectorWrapper::~OpenCvGPUFeatureDetectorWrapper()
{
    delete holder;
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
	oclMat mat0;
	oclMat mat1;
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

        DeviceInfo devInfo(idx);
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
    return 0;
}

bool OpenCvGPUFeatureDetectorProvider::provides(const DetectorType &type)
{
    if (cudaApi)
    {
        SWITCH_TYPE(SURF_GPU, return true;);
        SWITCH_TYPE(ORB_GPU , return true;);
    } else {
        SWITCH_TYPE(SURF_GPU, return true;);
    }
    
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
