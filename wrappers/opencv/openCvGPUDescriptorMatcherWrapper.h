#pragma once

#include "core/features2d/descriptorMatcherProvider.h"

#ifndef WITH_OPENCV_3x

namespace cv {
    namespace gpu {
        class BruteForceMatcher_GPU_base;
    }

    namespace ocl {
        class BruteForceMatcher_OCL_base;
    }
}

class OpenCvGPUDescriptorMatcherWrapper : public DescriptorMatcher
{
public:
    OpenCvGPUDescriptorMatcherWrapper( cv::gpu::BruteForceMatcher_GPU_base *matcher );
    OpenCvGPUDescriptorMatcherWrapper( cv::ocl::BruteForceMatcher_OCL_base *matcher );
    ~OpenCvGPUDescriptorMatcherWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;
    bool isParallelable() { return false; }

protected:
	void knnMatchImpl(corecvs::RuntimeTypeBuffer &query, corecvs::RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K);

private:
    OpenCvGPUDescriptorMatcherWrapper( const OpenCvGPUDescriptorMatcherWrapper &wrapper );

    cv::gpu::BruteForceMatcher_GPU_base *matcherBF_CUDA;
    cv::ocl::BruteForceMatcher_OCL_base *matcherBF_OCL;
};

class OpenCvGPUDescriptorMatcherProvider : public DescriptorMatcherProviderImpl
{
public:
    OpenCvGPUDescriptorMatcherProvider( bool cudaApi );

    DescriptorMatcher* getDescriptorMatcher(const DescriptorType &descriptor, const MatcherType &matcher, const std::string &params = "");
    bool provides(const DescriptorType &descriptor, const MatcherType &matcher);
    virtual std::string name() override { return "OpenCv GPU"; }

    ~OpenCvGPUDescriptorMatcherProvider() {}

    bool cudaApi;
};

#endif

extern "C"
{
    void init_opencv_gpu_matchers_provider();
}
