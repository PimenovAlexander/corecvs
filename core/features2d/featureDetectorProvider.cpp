#include "features2d/featureDetectorProvider.h"

#include "utils/global.h"

using namespace corecvs;
using namespace std;

FeatureDetector* FeatureDetectorProvider::getDetector(const DetectorType &type, const std::string &params)
{
    for (std::vector<FeatureDetectorProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        if ((*p)->provides(type))
        {
            return (*p)->getFeatureDetector(type, params);
        }
    }
    CORE_ASSERT_FAIL_P(("FeatureDetectorProvider::getDetector(%s): no providers", type.c_str()));
    return 0;
}


void FeatureDetector::detect(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeypoints, void* pRemapCache)
{
	detectImpl(image, keyPoints, nKeypoints, pRemapCache);
}

FeatureDetectorProvider::~FeatureDetectorProvider()
{

}


FeatureDetectorProvider& FeatureDetectorProvider::getInstance()
{
    static FeatureDetectorProvider provider;
    return provider;
}

FeatureDetectorProvider::FeatureDetectorProvider()
{
}
