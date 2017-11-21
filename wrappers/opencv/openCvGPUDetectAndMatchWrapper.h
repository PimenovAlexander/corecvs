#ifndef OPENCVGPUDETECTANDMATCHWRAPPER_H
#define OPENCVGPUDETECTANDMATCHWRAPPER_H

#ifdef WITH_OPENCV

#include "core/features2d/detectExtractAndMatchProvider.h"

namespace cv {
	namespace gpu {
		class SURF_GPU;
		class ORB_GPU;
        class BruteForceMatcher_GPU_base;
	}

	namespace ocl {
		class SURF_OCL;
        class BruteForceMatcher_OCL_base;
	}
}

class OpenCvGPUDetectExtractAndMatchWrapper : public DetectExtractAndMatch
{
public:
    OpenCvGPUDetectExtractAndMatchWrapper(cv::gpu::SURF_GPU *detector, cv::gpu::BruteForceMatcher_GPU_base* matcher);
    OpenCvGPUDetectExtractAndMatchWrapper(cv::gpu::ORB_GPU  *detector, cv::gpu::BruteForceMatcher_GPU_base* matcher);
    OpenCvGPUDetectExtractAndMatchWrapper(cv::ocl::SURF_OCL *detector, cv::ocl::BruteForceMatcher_OCL_base* matcher);

   ~OpenCvGPUDetectExtractAndMatchWrapper();

protected:
    void detectExtractAndMatchImpl(FeatureMatchingPipeline& pipeline, int nMaxKeypoints, int numResponcesPerPoint) override;

private:
	OpenCvGPUDetectExtractAndMatchWrapper(const OpenCvGPUDetectExtractAndMatchWrapper&);
  //OpenCvGPUDetectExtractAndMatchWrapper& operator=(const OpenCvGPUDetectExtractAndMatchWrapper&);

	cv::gpu::SURF_GPU* detectorSURF_CUDA;
	cv::gpu::ORB_GPU*  detectorORB_CUDA;
	cv::ocl::SURF_OCL* detectorSURF_OCL;

    cv::gpu::BruteForceMatcher_GPU_base* matcherBF_CUDA;
    cv::ocl::BruteForceMatcher_OCL_base* matcherBF_OCL;
};

extern "C"
{
    bool init_opencv_gpu_detect_extract_and_match_provider(bool& cudaApi);
}

class OpenCvGPUDetectExtractAndMatchProvider : public DetectExtractAndMatchProviderImpl
{
public:
    OpenCvGPUDetectExtractAndMatchProvider(bool cudaApi);

   ~OpenCvGPUDetectExtractAndMatchProvider() {}

    DetectExtractAndMatch* getDetector(const DetectorType &detectorType, const DescriptorType &descriptorType, const MatcherType &matcherType, const std::string &params = "") override;

    bool    provides(const DetectorType &detectorType, const DescriptorType &descriptorType, const MatcherType &matcherType) override;

    double getProperty(const std::string &name) const override;
    void   setProperty(const std::string &name, const double &value) override;

    bool isParallelable() override { return false; }

private:
	bool    cudaApi; // opencv module to use : cuda or opencl
};

#endif // WITH_OPENCV

#endif // OPENCVGPUDETECTANDMATCHWRAPPER_H
