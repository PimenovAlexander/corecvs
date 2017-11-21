#pragma once

#include "core/features2d/descriptorExtractorProvider.h"

namespace cv {
#ifdef WITH_OPENCV_3x
	class Feature2D;
	typedef Feature2D DescriptorExtractor;
#else
	class DescriptorExtractor;
#endif  
}

#ifdef WITH_OPENCV_3x
struct SmartPtrExtractorHolder;
#endif

class OpenCvDescriptorExtractorWrapper : public DescriptorExtractor
{
public:
#ifdef WITH_OPENCV_3x
    OpenCvDescriptorExtractorWrapper(SmartPtrExtractorHolder *holder);
#else
    OpenCvDescriptorExtractorWrapper(cv::DescriptorExtractor *detector);
#endif
   ~OpenCvDescriptorExtractorWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

protected:
    void computeImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descripors, void* pRemapCache);

private:
    OpenCvDescriptorExtractorWrapper(const OpenCvDescriptorExtractorWrapper &wrapper);

    cv::DescriptorExtractor *extractor;

#ifdef WITH_OPENCV_3x
    SmartPtrExtractorHolder /*SmartPtrHolder*/ * holder;   // EM/SF: opencv3.2 requires it?
#endif
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
