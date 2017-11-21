#ifdef WITH_OPENCV

#pragma once

#include "core/features2d/detectAndExtractProvider.h"

struct DetectorHolder;

class OpenCvDetectAndExtractWrapper : public DetectAndExtract
{
public:
    OpenCvDetectAndExtractWrapper(DetectorHolder *holder);
   ~OpenCvDetectAndExtractWrapper();

    void   setProperty(const std::string &name, const double &value);
    double getProperty(const std::string &name) const;

    bool isParallelable() { return true; }

protected:
	void detectAndExtractImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descriptors, int nMaxKeypoints, void* pRemapCache);

private:
    OpenCvDetectAndExtractWrapper(const OpenCvDetectAndExtractWrapper &wrapper);
    OpenCvDetectAndExtractWrapper& operator=( const OpenCvDetectAndExtractWrapper& );
    DetectorHolder* holder;
};

extern "C"
{
    void init_opencv_detect_and_extract_provider();
}

class OpenCvDetectAndExtractProvider : public DetectAndExtractProviderImpl
{
public:
    OpenCvDetectAndExtractProvider();

    ~OpenCvDetectAndExtractProvider() {}

    DetectAndExtract* getDetector( const DetectorType &detectorType, const DescriptorType &descriptorType, const std::string &params = "" );
    bool provides(const DetectorType &detectorType, const DescriptorType &descriptorType);
    virtual std::string name()  override { return "OpenCV"; }
    //virtual std::vector<std::string> provideHints() override;

};

#endif
