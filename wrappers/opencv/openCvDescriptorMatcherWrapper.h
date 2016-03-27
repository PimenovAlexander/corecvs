#pragma once

#include "descriptorMatcherProvider.h"

namespace cv {
    class DescriptorMatcher;
};

class OpenCvDescriptorMatcherWrapper : public DescriptorMatcher
{
public:
    OpenCvDescriptorMatcherWrapper(cv::DescriptorMatcher *detector);
   ~OpenCvDescriptorMatcherWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

protected:
    void knnMatchImpl(RuntimeTypeBuffer &query, RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K);

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
    DescriptorMatcher* getDescriptorMatcher(const DescriptorType &descriptor, const MatcherType &matcher);
    bool provides(const DescriptorType &descriptor, const MatcherType &matcher);

    ~OpenCvDescriptorMatcherProvider() {}
};
