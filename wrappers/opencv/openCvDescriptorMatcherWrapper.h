#pragma once

#include "core/features2d/descriptorMatcherProvider.h"

namespace cv {
    class DescriptorMatcher;
}

class OpenCvDescriptorMatcherWrapper : public DescriptorMatcher
{
public:
    OpenCvDescriptorMatcherWrapper(cv::DescriptorMatcher *detector);
   ~OpenCvDescriptorMatcherWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

protected:
    void knnMatchImpl(corecvs::RuntimeTypeBuffer &query, corecvs::RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K);

private:
    OpenCvDescriptorMatcherWrapper(const OpenCvDescriptorMatcherWrapper &wrapper);

    cv::DescriptorMatcher *matcher;
};

extern "C"
{
    void init_opencv_matchers_provider();
}

class OpenCvDescriptorMatcherProvider : public DescriptorMatcherProviderImpl
{
public:
    DescriptorMatcher* getDescriptorMatcher(const DescriptorType &descriptor, const MatcherType &matcher, const std::string &params = "");
    virtual bool provides(const DescriptorType &descriptor, const MatcherType &matcher) override;
    virtual std::string name() override {return "OpenCV";}
    virtual std::vector<std::string> provideHints() override;


    ~OpenCvDescriptorMatcherProvider() {}
};
