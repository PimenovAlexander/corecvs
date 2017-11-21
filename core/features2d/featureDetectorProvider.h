#pragma once

#include "core/features2d/imageKeyPoints.h"
#include "core/features2d/algoBase.h"

class FeatureDetector : public virtual AlgoBase
{
public:
    void detect(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeypoints, void* pRemapCache);
    virtual ~FeatureDetector() {}

protected:
    virtual void detectImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeypoints, void* pRemapCache) = 0;
};

class FeatureDetectorProviderImpl : public AlgoNaming
{
public:
    virtual FeatureDetector* getFeatureDetector(const DetectorType &type, const std::string &params = "") = 0;
    virtual bool provides(const DetectorType &type) = 0;

    virtual ~FeatureDetectorProviderImpl() {}
};

class FeatureDetectorProvider : public AlgoCollectionNaming<FeatureDetectorProviderImpl>
{
public:
    static FeatureDetectorProvider& getInstance();
    ~FeatureDetectorProvider();

    FeatureDetector* getDetector(const DetectorType &type, const std::string &params = "");


private:
    FeatureDetectorProvider();
    FeatureDetectorProvider(const FeatureDetectorProvider&);
    FeatureDetectorProvider& operator=(const FeatureDetectorProvider&);

};
