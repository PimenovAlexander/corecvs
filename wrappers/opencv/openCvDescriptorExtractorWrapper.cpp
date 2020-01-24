#include "openCvDescriptorExtractorWrapper.h"
#include "openCvKeyPointsWrapper.h"
#include "openCvDefaultParams.h"

#include "core/utils/global.h"

#include <opencv2/features2d/features2d.hpp>    // cv::DescriptorExtractor
#include <opencv2/imgproc/imgproc.hpp>			// cv::remap

#   include <opencv2/xfeatures2d/nonfree.hpp>      // cv::xfeatures2d::SURF, cv::xfeatures2d::SIFT
#   include <opencv2/xfeatures2d.hpp>				// cv::xfeatures2d::STAR


struct SmartPtrExtractorHolder
{
    SmartPtrExtractorHolder() : tag(SIFT), sift() {}
    ~SmartPtrExtractorHolder() {}
    enum {
        SIFT, SURF, BRISK, ORB, AKAZE
    } tag;

#if 1    // EM/SF: opencv3.2 requires it?
    union {
#else
    struct {
#endif
        cv::Ptr< cv::xfeatures2d::SIFT >            sift;
        cv::Ptr< cv::xfeatures2d::SURF >            surf;
        cv::Ptr< cv::BRISK >                        brisk;
        cv::Ptr< cv::ORB >                          orb;
        cv::Ptr< cv::AKAZE >                        akaze;
    };

    cv::DescriptorExtractor *get() {
        switch (tag) {
        case SIFT:
            return sift.get();
        case SURF:
            return surf.get();
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

OpenCvDescriptorExtractorWrapper::OpenCvDescriptorExtractorWrapper(SmartPtrExtractorHolder *holder) : holder(holder)
{
    extractor = holder->get();
}

OpenCvDescriptorExtractorWrapper::~OpenCvDescriptorExtractorWrapper()
{
    delete holder;
}

struct CvRemapCache
{
	cv::Mat mat0;
	cv::Mat mat1;
};

void OpenCvDescriptorExtractorWrapper::computeImpl(RuntimeTypeBuffer &image
    , std::vector<KeyPoint> &keyPoints
    , RuntimeTypeBuffer &descriptors
	, void* pRemapCache )
{
    std::vector<cv::KeyPoint> kps;
    kps.reserve(keyPoints.size());
    FOREACH(const KeyPoint& kp, keyPoints)
    {
        kps.emplace_back(convert(kp));
    }
    cv::Mat img = convert(image), desc;
	if (pRemapCache)
	{
		cv::Mat remapped;
		CvRemapCache* p = (CvRemapCache*)(pRemapCache);
		cv::remap(img, remapped, p->mat0, p->mat1, cv::INTER_NEAREST);
		img = remapped;
	}

    extractor->compute(img, kps, desc);

    keyPoints.clear();
    keyPoints.reserve(kps.size());
    FOREACH(const cv::KeyPoint& kp, kps)
    {
        keyPoints.emplace_back(convert(kp));
    }

    descriptors = convert(desc);
}

void OpenCvDescriptorExtractorWrapper::setProperty(const std::string &name, const double &value)
{
    CORE_UNUSED(name);
    CORE_UNUSED(value);
}

double OpenCvDescriptorExtractorWrapper::getProperty(const std::string &name) const
{
    CORE_UNUSED(name);
    return 0.0;
}

void init_opencv_descriptors_provider()
{
    DescriptorExtractorProvider::getInstance().add(new OpenCvDescriptorExtractorProvider());
}

#define SWITCH_TYPE(str, expr) \
    if(type == #str) \
    { \
        expr; \
    }

DescriptorExtractor* OpenCvDescriptorExtractorProvider::getDescriptorExtractor(const DescriptorType &type, const std::string &params)
{
    SiftParams siftParams(params);
    SurfParams surfParams(params);
    BriskParams briskParams(params);
    OrbParams orbParams(params);
    AkazeParams akazeParams(params);


    SmartPtrExtractorHolder* holder = new SmartPtrExtractorHolder;
    if (type == "SIFT")
    {
        cv::Ptr< cv::xfeatures2d::SIFT > ptr = cv::xfeatures2d::SIFT::create(0, siftParams.nOctaveLayers, siftParams.contrastThreshold, siftParams.edgeThreshold, siftParams.sigma);
        holder->set(ptr);
        return new OpenCvDescriptorExtractorWrapper(holder);
    }

    if (type == "SURF")
    {
        cv::Ptr< cv::xfeatures2d::SURF > ptr = cv::xfeatures2d::SURF::create(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, surfParams.upright);
        holder->set(ptr);
        return new OpenCvDescriptorExtractorWrapper(holder);
    }

    if (type == "BRISK")
    {
        cv::Ptr< cv::BRISK > ptr = cv::BRISK::create(briskParams.thresh, briskParams.octaves, briskParams.patternScale);
        holder->set(ptr);
        return new OpenCvDescriptorExtractorWrapper(holder);
    }

    if (type == "ORB")
    {
        cv::Ptr< cv::ORB > ptr = cv::ORB::create(orbParams.maxFeatures, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize);
        holder->set(ptr);
        return new OpenCvDescriptorExtractorWrapper(holder);
    }

    if (type == "AKAZE")
    {
        cv::Ptr< cv::AKAZE > ptr = cv::AKAZE::create(akazeParams.descriptorType, akazeParams.descriptorSize, akazeParams.descriptorChannels, akazeParams.threshold, akazeParams.octaves, akazeParams.octaveLayers, akazeParams.diffusivity);
        holder->set(ptr);
        return new OpenCvDescriptorExtractorWrapper(holder);
    }
    return 0;
}

bool OpenCvDescriptorExtractorProvider::provides(const DescriptorType &type)
{
    SWITCH_TYPE(SIFT, return true;);
    SWITCH_TYPE(SURF, return true;);
    SWITCH_TYPE(BRISK, return true;);
    SWITCH_TYPE(ORB, return true;);
    SWITCH_TYPE(AKAZE, return true;);
    return false;
}

std::vector<std::string> OpenCvDescriptorExtractorProvider::provideHints()
{
    std::vector<std::string> result;

    result.push_back("SIFT");
    result.push_back("SURF");
    result.push_back("BRISK");
    result.push_back("ORB");
    result.push_back("AKAZE");
    return result;
}

#undef SWITCH_TYPE
