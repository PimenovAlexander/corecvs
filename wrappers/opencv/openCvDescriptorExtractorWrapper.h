#pragma once

#include "descriptorExtractorProvider.h"

namespace cv {
    class DescriptorExtractor;
};

class OpenCvDescriptorExtractorWrapper : public DescriptorExtractor
{
public:
    OpenCvDescriptorExtractorWrapper(cv::DescriptorExtractor *detector);
   ~OpenCvDescriptorExtractorWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

protected:
    void computeImpl(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, RuntimeTypeBuffer &descripors);

private:
    OpenCvDescriptorExtractorWrapper(const OpenCvDescriptorExtractorWrapper &wrapper);

    cv::DescriptorExtractor *extractor;
};

extern "C"
{
    void init_opencv_descriptors_provider();
}

class OpenCvDescriptorExtractorProvider : public DescriptorExtractorProviderImpl
{
public:
    DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type);
    bool provides(const DescriptorType &type);

    ~OpenCvDescriptorExtractorProvider() {}
};
