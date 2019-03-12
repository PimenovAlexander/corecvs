#pragma once

#include "core/features2d/descriptorExtractorProvider.h"

#ifndef WITH_OPENCV_3

namespace cv {
    namespace gpu {
        class SURF_GPU;
        class ORB_GPU;
    }

    namespace ocl {
        class SURF_OCL;
    }
}

class OpenCvGPUDescriptorExtractorWrapper : public DescriptorExtractor
{
public:
    OpenCvGPUDescriptorExtractorWrapper( cv::gpu::SURF_GPU *extractor );
    OpenCvGPUDescriptorExtractorWrapper( cv::gpu::ORB_GPU *extractor );
    OpenCvGPUDescriptorExtractorWrapper( cv::ocl::SURF_OCL* extractor );
    ~OpenCvGPUDescriptorExtractorWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;
    bool isParallelable() { return false; }

protected:
	void computeImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descripors, void* pRemapCache);

private:
    OpenCvGPUDescriptorExtractorWrapper( const OpenCvGPUDescriptorExtractorWrapper &wrapper );

    cv::gpu::SURF_GPU *extractorSURF_CUDA;
    cv::gpu::ORB_GPU *extractorORB_CUDA;
    cv::ocl::SURF_OCL *extractorSURF_OCL;
};

class OpenCvGPUDescriptorExtractorProvider : public DescriptorExtractorProviderImpl
{
public:
    OpenCvGPUDescriptorExtractorProvider( bool cudaApi );
    DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type, const std::string &params = "");
    bool provides(const DescriptorType &type);
    virtual std::string name() override { return "OpenCv GPU"; }

    ~OpenCvGPUDescriptorExtractorProvider() {}

    bool cudaApi;
};

#endif

extern "C"
{
    void init_opencv_gpu_descriptors_provider();
}
