#pragma once

#include "core/features2d/descriptorExtractorProvider.h"

namespace cv {
	class Feature2D;
	typedef Feature2D DescriptorExtractor;
}

struct SmartPtrExtractorHolder;

class OpenCvDescriptorExtractorWrapper : public DescriptorExtractor
{
public:
    OpenCvDescriptorExtractorWrapper(SmartPtrExtractorHolder *holder);
   ~OpenCvDescriptorExtractorWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

protected:
    void computeImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descripors, void* pRemapCache);

private:
    OpenCvDescriptorExtractorWrapper(const OpenCvDescriptorExtractorWrapper &wrapper);

    cv::DescriptorExtractor *extractor;

    SmartPtrExtractorHolder /*SmartPtrHolder*/ * holder;   // EM/SF: opencv3.2 requires it?
};

extern "C"
{
    void init_opencv_descriptors_provider();
}

class OpenCvDescriptorExtractorProvider : public DescriptorExtractorProviderImpl
{
public:
    DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type, const std::string &params = "");
    virtual bool provides(const DescriptorType &type) override;
    virtual std::string name() override { return "OpenCV"; }
    virtual std::vector<std::string> provideHints() override;


    ~OpenCvDescriptorExtractorProvider() {}

};
