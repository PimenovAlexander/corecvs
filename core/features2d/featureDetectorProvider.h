#pragma once

#include "core/features2d/imageKeyPoints.h"
#include "core/features2d/algoBase.h"

class FeatureDetector : public virtual AlgoBase
{
public:
	void detect(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints);
	virtual ~FeatureDetector() {}

protected:
	virtual void detectImpl(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints) = 0;
};

class FeatureDetectorProviderImpl
{
public:
	virtual FeatureDetector* getFeatureDetector(const DetectorType &type) = 0;
	virtual bool provides(const DetectorType &type) = 0;

	virtual ~FeatureDetectorProviderImpl() {}
};

class FeatureDetectorProvider
{
public:
	static FeatureDetectorProvider& getInstance();
	~FeatureDetectorProvider();

	void add(FeatureDetectorProviderImpl *provider);
	FeatureDetector* getDetector(const DetectorType &type);

private:
	FeatureDetectorProvider();
	FeatureDetectorProvider(const FeatureDetectorProvider&);
	FeatureDetectorProvider& operator=(const FeatureDetectorProvider&);

	std::vector<FeatureDetectorProviderImpl*> providers;
};
