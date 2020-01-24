#pragma once

#include "core/features2d/featureDetectorProvider.h"

namespace cv {
    namespace cuda {
        class ORB;
        class SURF_CUDA;
    }
}

struct SmartPtrHolder;

class OpenCvGPUFeatureDetectorWrapper : public FeatureDetector
{
public:

    OpenCvGPUFeatureDetectorWrapper(cv::cuda::SURF_CUDA *detector);
    OpenCvGPUFeatureDetectorWrapper(SmartPtrHolder *holder);

    ~OpenCvGPUFeatureDetectorWrapper();

    double getProperty(const std::string &name) const;
    void   setProperty(const std::string &name, const double &value);

    bool  isParallelable() { return false; }

protected:
	void detectImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeypoints, void* pRemapCache);

private:
    OpenCvGPUFeatureDetectorWrapper( const OpenCvGPUFeatureDetectorWrapper& );
    OpenCvGPUFeatureDetectorWrapper& operator=( const OpenCvGPUFeatureDetectorWrapper& );

    SmartPtrHolder*      holder;
    cv::cuda::SURF_CUDA* detectorSURF_CUDA;
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
