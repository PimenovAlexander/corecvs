#pragma once

#include "core/features2d/featureDetectorProvider.h"

namespace cv {
#ifdef WITH_OPENCV_3x
    namespace cuda {
        class ORB;
        class SURF_CUDA;
    }
#else
    namespace gpu {
        class SURF_GPU;
        class ORB_GPU;
    } 

    namespace ocl {
        class SURF_OCL;
    }
#endif
}

#ifdef WITH_OPENCV_3x
    struct SmartPtrHolder;
#endif

class OpenCvGPUFeatureDetectorWrapper : public FeatureDetector
{
public:

#ifdef WITH_OPENCV_3x
    OpenCvGPUFeatureDetectorWrapper(cv::cuda::SURF_CUDA *detector);
    OpenCvGPUFeatureDetectorWrapper(SmartPtrHolder *holder);
#else
    OpenCvGPUFeatureDetectorWrapper( cv::gpu::SURF_GPU *detector );
    OpenCvGPUFeatureDetectorWrapper( cv::gpu::ORB_GPU *detector );
    OpenCvGPUFeatureDetectorWrapper( cv::ocl::SURF_OCL *detector );
#endif

    ~OpenCvGPUFeatureDetectorWrapper();

    double getProperty(const std::string &name) const;
    void   setProperty(const std::string &name, const double &value);

    bool  isParallelable() { return false; }

protected:
	void detectImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeypoints, void* pRemapCache);

private:
    OpenCvGPUFeatureDetectorWrapper( const OpenCvGPUFeatureDetectorWrapper& );
    OpenCvGPUFeatureDetectorWrapper& operator=( const OpenCvGPUFeatureDetectorWrapper& );

#ifdef WITH_OPENCV_3x
    SmartPtrHolder*      holder;
    cv::cuda::SURF_CUDA* detectorSURF_CUDA;
#else
    cv::gpu::SURF_GPU* detectorSURF_CUDA;
    cv::gpu::ORB_GPU*  detectorORB_CUDA;
    cv::ocl::SURF_OCL* detectorSURF_OCL;
#endif

};

extern "C"
{
    void init_opencv_gpu_detectors_provider();
}

class OpenCvGPUFeatureDetectorProvider : public FeatureDetectorProviderImpl
{
public:
    OpenCvGPUFeatureDetectorProvider( bool cudaApi );

    ~OpenCvGPUFeatureDetectorProvider() {}

    FeatureDetector* getFeatureDetector(const DetectorType &type, const std::string &params = "");
    bool provides(const DetectorType &type);
    virtual std::string name() override { return "OpenCV GPU"; }
    virtual std::vector<std::string> provideHints();

private:
    bool               cudaApi; // opencv module to use : cuda or opencl
};
