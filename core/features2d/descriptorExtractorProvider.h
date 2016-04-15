#pragma once

#include "imageKeyPoints.h"
#include "algoBase.h"


class DescriptorExtractor : public virtual AlgoBase
{
public:
	void compute(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, RuntimeTypeBuffer &descriptors);
	virtual ~DescriptorExtractor() {}

protected:
	virtual void computeImpl(RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, RuntimeTypeBuffer &descriptors) = 0;
};

class DescriptorExtractorProviderImpl
{
public:
	virtual DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type) = 0;
	virtual bool provides(const DescriptorType &type) = 0;

	virtual ~DescriptorExtractorProviderImpl() {}
};

class DescriptorExtractorProvider
{
public:
	void add(DescriptorExtractorProviderImpl *provider);
	DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type);

	static DescriptorExtractorProvider& getInstance();
	~DescriptorExtractorProvider();

private:
	DescriptorExtractorProvider();
	DescriptorExtractorProvider(const DescriptorExtractorProvider&);
	DescriptorExtractorProvider& operator=(const DescriptorExtractorProvider&);

	std::vector<DescriptorExtractorProviderImpl*> providers;
};
