#pragma once

#include "core/features2d/featureDetectorProvider.h"

namespace cv {
#ifdef WITH_OPENCV_3x
	class Feature2D;
	typedef Feature2D FeatureDetector;
#else
	class FeatureDetector;
#endif  
}

#ifdef WITH_OPENCV_3x
    struct SmartPtrDetectorHolder;
#endif

class OpenCvFeatureDetectorWrapper : public FeatureDetector
{
public:
#ifdef WITH_OPENCV_3x
	OpenCvFeatureDetectorWrapper(SmartPtrDetectorHolder *holder);
#else
    OpenCvFeatureDetectorWrapper(cv::FeatureDetector *detector);  
#endif  
    
   ~OpenCvFeatureDetectorWrapper();

    double getProperty(const std::string &name) const;
    void   setProperty(const std::string &name, const double &value);

protected:
    void detectImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nMax, void* pRemapCache);

private:
    OpenCvFeatureDetectorWrapper(const OpenCvFeatureDetectorWrapper&);
    OpenCvFeatureDetectorWrapper& operator=(const OpenCvFeatureDetectorWrapper&);

    cv::FeatureDetector* detector;
#ifdef WITH_OPENCV_3x
    SmartPtrDetectorHolder* holder;
#endif  
};

extern "C"
{
    void init_opencv_detectors_provider();
}

class OpenCvFeatureDetectorProvider : public FeatureDetectorProviderImpl
{
public:
    FeatureDetector* getFeatureDetector(const DetectorType &type, const std::string &params = "");
    virtual bool provides(const DetectorType &type) override;
    virtual std::string name()  override {return "OpenCV"; }
    virtual std::vector<std::string> provideHints() override;

    ~OpenCvFeatureDetectorProvider() {}
};
