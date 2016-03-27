#include "openCvDescriptorExtractorWrapper.h"
#include "openCvKeyPointsWrapper.h"
#include "openCvDefaultParams.h"

#include "global.h"

#include <opencv2/features2d/features2d.hpp>    // cv::DescriptorExtractor
#include <opencv2/nonfree/features2d.hpp>       // cv::SURF

OpenCvDescriptorExtractorWrapper::OpenCvDescriptorExtractorWrapper(cv::DescriptorExtractor *extractor)
    : extractor(extractor)
{}

OpenCvDescriptorExtractorWrapper::~OpenCvDescriptorExtractorWrapper()
{
	delete extractor;
}

void OpenCvDescriptorExtractorWrapper::computeImpl(RuntimeTypeBuffer &image
    , std::vector<KeyPoint> &keyPoints
    , RuntimeTypeBuffer &descriptors)
{
	std::vector<cv::KeyPoint> kps;
	FOREACH(const KeyPoint& kp, keyPoints)
	{
		kps.push_back(convert(kp));
	}
	cv::Mat img = convert(image), desc;

	extractor->compute(img, kps, desc);

	keyPoints.clear();
	FOREACH(const cv::KeyPoint& kp, kps)
	{
		keyPoints.push_back(convert(kp));
	}

	descriptors = convert(desc);
}

void OpenCvDescriptorExtractorWrapper::setProperty(const std::string &name, const double &value)
{
	extractor->set(name, value);
}

double OpenCvDescriptorExtractorWrapper::getProperty(const std::string &name) const
{
	return extractor->get<double>(name);
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

DescriptorExtractor* OpenCvDescriptorExtractorProvider::getDescriptorExtractor(const DescriptorType &type)
{
	SiftParams siftParams;
	SurfParams surfParams;
	BriskParams briskParams;
	OrbParams orbParams;
	SWITCH_TYPE(SIFT,
			return new OpenCvDescriptorExtractorWrapper(new cv::SIFT(0, siftParams.nOctaveLayers, siftParams.contrastThreshold, siftParams.edgeThreshold, siftParams.sigma));)
		SWITCH_TYPE(SURF,
				return new OpenCvDescriptorExtractorWrapper(new cv::SURF(surfParams.hessianThreshold, surfParams.octaves, surfParams.octaveLayers, surfParams.extended, surfParams.upright));)
		SWITCH_TYPE(BRISK,
				return new OpenCvDescriptorExtractorWrapper(new cv::BRISK(briskParams.thresh, briskParams.octaves, briskParams.patternScale));)
		SWITCH_TYPE(ORB,
				return new OpenCvDescriptorExtractorWrapper(new cv::ORB(2000, orbParams.scaleFactor, orbParams.nLevels, orbParams.edgeThreshold, orbParams.firstLevel, orbParams.WTA_K, orbParams.scoreType, orbParams.patchSize));)
		return 0;
}

bool OpenCvDescriptorExtractorProvider::provides(const DescriptorType &type)
{
	SWITCH_TYPE(SIFT, return true;);
	SWITCH_TYPE(SURF, return true;);
	SWITCH_TYPE(BRISK, return true;);
	SWITCH_TYPE(ORB, return true;);
	return false;
}

#undef SWITCH_TYPE
