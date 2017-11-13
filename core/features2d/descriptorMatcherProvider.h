#pragma once

#include "core/features2d/descriptorExtractorProvider.h"
#include "core/features2d/imageMatches.h"

class DescriptorMatcher : public virtual AlgoBase
{
public:
	void knnMatch(RuntimeTypeBuffer &query, RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K);
	virtual ~DescriptorMatcher() {}
protected:
	virtual void knnMatchImpl(RuntimeTypeBuffer &query, RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K) = 0;
};


class DescriptorMatcherProviderImpl
{
public:
	virtual DescriptorMatcher* getDescriptorMatcher(const DescriptorType &descriptor, const MatcherType &matcher) = 0;
	virtual bool provides(const DescriptorType &descriptor, const MatcherType &matcher) = 0;

	virtual ~DescriptorMatcherProviderImpl() {}
};

class DescriptorMatcherProvider
{
public:
	void add(DescriptorMatcherProviderImpl *provider);
	DescriptorMatcher* getMatcher(const DescriptorType &descriptor, const MatcherType &matcher);
	static DescriptorMatcherProvider& getInstance();

	~DescriptorMatcherProvider();

private:
	DescriptorMatcherProvider();
	DescriptorMatcherProvider(const DescriptorMatcherProvider&);
	DescriptorMatcherProvider& operator=(const DescriptorMatcherProvider&);

	std::vector<DescriptorMatcherProviderImpl*> providers;
};
