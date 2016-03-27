#pragma once

#include "featureDetectorProvider.h"

namespace cv {
    class FeatureDetector;
};

class OpenCvFeatureDetectorWrapper : public FeatureDetector
{
public:
    OpenCvFeatureDetectorWrapper(cv::FeatureDetector *detector);
   ~OpenCvFeatureDetectorWrapper();

    double getProperty(const std::string &name) const;
    void   setProperty(const std::string &name, const double &value);

protected:
    void detectImpl(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints);

private:
    OpenCvFeatureDetectorWrapper(const OpenCvFeatureDetectorWrapper&);
    OpenCvFeatureDetectorWrapper& operator=(const OpenCvFeatureDetectorWrapper&);

    cv::FeatureDetector* detector;
};

extern "C"
{
    void init_opencv_detectors_provider();
}

class OpenCvFeatureDetectorProvider : public FeatureDetectorProviderImpl
{
public:
    FeatureDetector* getFeatureDetector(const DetectorType &type);
    bool provides(const DetectorType &type);

    ~OpenCvFeatureDetectorProvider() {}
};
