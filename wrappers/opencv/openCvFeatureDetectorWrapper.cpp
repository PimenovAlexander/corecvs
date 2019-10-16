#include "openCvFeatureDetectorWrapper.h"
#include "openCvKeyPointsWrapper.h"

#include "openCvDefaultParams.h"

#include "core/utils/global.h"

#include <opencv2/features2d/features2d.hpp>    // cv::FeatureDetector
#include <opencv2/imgproc/imgproc.hpp>			// cv::remap

#include <opencv2/xfeatures2d/nonfree.hpp>      // cv::xfeatures2d::SURF, cv::xfeatures2d::SIFT
#include <opencv2/xfeatures2d.hpp>				// cv::xfeatures2d::STAR

using namespace corecvs;

struct SmartPtrDetectorHolder
{
    SmartPtrDetectorHolder() : tag(SIFT), sift() {}
    ~SmartPtrDetectorHolder() {}
    enum {
        SIFT, SURF, STAR, FAST, BRISK, ORB, AKAZE
    } tag;

#if 1    // EM/SF: opencv3.2 requires it?
    union {
#else
    struct {
#endif
        cv::Ptr< cv::xfeatures2d::SIFT >            sift;
        cv::Ptr< cv::xfeatures2d::SURF >            surf;
        cv::Ptr< cv::xfeatures2d::StarDetector >	star;
        cv::Ptr< cv::FastFeatureDetector>           fast;
        cv::Ptr< cv::BRISK >                        brisk;
        cv::Ptr< cv::ORB >                          orb;
        cv::Ptr< cv::AKAZE >                        akaze;
    };

    cv::FeatureDetector *get() {
        switch (tag) {
        case SIFT:
            return sift.get();
        case SURF:
            return surf.get();
        case STAR:
            return star.get();
        case FAST:
            return fast.get();
        case BRISK:
            return brisk.get();
        case ORB:
            return orb.get();
        case AKAZE:
            return akaze.get();
        default:
            return nullptr;
        }
    }

    void set(cv::Ptr<cv::xfeatures2d::SIFT> value) {
        tag = SIFT;
        sift = value;
    }
    void set(cv::Ptr<cv::xfeatures2d::SURF> value) {
        tag = SURF;
        surf = value;
    }
    void set(cv::Ptr<cv::xfeatures2d::StarDetector> value) {
        tag = STAR;
        star = value;
    }
    void set(cv::Ptr<cv::FastFeatureDetector> value) {
        tag = FAST;
        fast = value;
    }
    void set(cv::Ptr<cv::BRISK> value) {
        tag = BRISK;
        brisk = value;
    }
    void set(cv::Ptr<cv::ORB> value) {
        tag = ORB;
        orb = value;
    }
    void set(cv::Ptr<cv::AKAZE> value) {
        tag = AKAZE;
        akaze = value;
    }
};

OpenCvFeatureDetectorWrapper::OpenCvFeatureDetectorWrapper(SmartPtrDetectorHolder *holder) : holder(holder)
{
    detector = holder->get();
}

OpenCvFeatureDetectorWrapper::~OpenCvFeatureDetectorWrapper()
{
    delete holder;
}

double OpenCvFeatureDetectorWrapper::getProperty(const std::string &name) const
{
	CORE_UNUSED(name);
	return 0.0;
}

void OpenCvFeatureDetectorWrapper::setProperty(const std::string &name, const double &value)
{
	CORE_UNUSED(name);
	CORE_UNUSED(value);
}

struct CvRemapCache
{
	cv::Mat mat0;
	cv::Mat mat1;
};

void OpenCvFeatureDetectorWrapper::detectImpl(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, int nKeyPoints, void* pRemapCache)
{
	std::vector<cv::KeyPoint> kps;
    cv::Mat img = convert(image);
	if (pRemapCache)
	{
		cv::Mat remapped;
		CvRemapCache* p = (CvRemapCache*)(pRemapCache);
		cv::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
		img = remapped;
	}

    detector->detect(img, kps);

	keyPoints.clear();

    FOREACH(const cv::KeyPoint &kp, kps)
    {
		keyPoints.push_back(convert(kp));
    }
	std::sort(keyPoints.begin(), keyPoints.end(), [](const KeyPoint &l, const KeyPoint &r) { return l.response > r.response; });
	keyPoints.resize(std::min((int)keyPoints.size(), nKeyPoints));
}

void init_opencv_detectors_provider()
{
	FeatureDetectorProvider::getInstance().add(new OpenCvFeatureDetectorProvider());
}

#define SWITCH_TYPE(str, expr) \
	if(type == #str) \
	{ \
		expr; \
	}

FeatureDetector* OpenCvFeatureDetectorProvider::getFeatureDetector(const DetectorType &type, const std::string &params)
{
	SiftParams siftParams(params);
	SurfParams surfParams(params);
	StarParams starParams(params);
	FastParams fastParams(params);
	BriskParams briskParams(params);
	OrbParams orbParams(params);
    AkazeParams akazeParams(params);

	SmartPtrDetectorHolder* holder = new SmartPtrDetectorHolder;
	if (type == "SIFT")
	{
		cv::Ptr< cv::xfeatures2d::SIFT > ptr = cv::xfeatures2d::SIFT::create(0, siftParams.nOctaveLayers, siftParams.contrastThreshold, siftParams.edgeThreshold, siftParams.sigma);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
	}

    if (type == "SURF")
    {
        cv::Ptr< cv::xfeatures2d::SURF > ptr = cv::xfeatures2d::SURF::create(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, surfParams.upright);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }

    if (type == "STAR")
    {
        cv::Ptr< cv::xfeatures2d::StarDetector > ptr = cv::xfeatures2d::StarDetector::create(starParams.maxSize, starParams.responseThreshold, starParams.lineThresholdProjected, starParams.lineThresholdBinarized, starParams.supressNonmaxSize);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }

    if (type == "FAST")
    {
        cv::Ptr< cv::FastFeatureDetector > ptr = cv::FastFeatureDetector::create(fastParams.threshold, fastParams.nonmaxSuppression);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }

    if (type == "BRISK")
    {
        cv::Ptr< cv::BRISK > ptr = cv::BRISK::create(briskParams.thresh, briskParams.octaves, briskParams.patternScale);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }

    if (type == "ORB")
    {
        cv::Ptr< cv::ORB > ptr = cv::ORB::create(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }

    if (type == "AKAZE")
    {
        cv::Ptr< cv::AKAZE > ptr = cv::AKAZE::create(akazeParams.descriptorType, akazeParams.descriptorSize, akazeParams.descriptorChannels, akazeParams.threshold, akazeParams.octaves, akazeParams.octaveLayers, akazeParams.diffusivity);
        holder->set(ptr);
        return new OpenCvFeatureDetectorWrapper(holder);
    }
	return 0;
}

bool OpenCvFeatureDetectorProvider::provides(const DetectorType &type)
{
	SWITCH_TYPE(SIFT, return true;);
	SWITCH_TYPE(SURF, return true;);
	SWITCH_TYPE(STAR, return true;);
	SWITCH_TYPE(FAST, return true;);
	SWITCH_TYPE(BRISK, return true;);
	SWITCH_TYPE(ORB, return true;);
    SWITCH_TYPE(AKAZE, return true;);
    return false;
}

std::vector<std::string> OpenCvFeatureDetectorProvider::provideHints()
{
    std::vector<std::string> toReturn;
    toReturn.push_back("SIFT");
    toReturn.push_back("SURF");
    toReturn.push_back("STAR");
    toReturn.push_back("FAST");
    toReturn.push_back("BRISK");
    toReturn.push_back("ORB");
    toReturn.push_back("AKAZE");
    return toReturn;
}

#undef SWITCH_TYPE
