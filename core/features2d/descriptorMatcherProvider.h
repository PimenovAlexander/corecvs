#pragma once

#include "features2d/descriptorExtractorProvider.h"
#include "features2d/imageMatches.h"

class DescriptorMatcher : public virtual AlgoBase
{
public:
    void knnMatch(corecvs::RuntimeTypeBuffer &query, corecvs::RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K);
    virtual ~DescriptorMatcher() {}
protected:
    virtual void knnMatchImpl(corecvs::RuntimeTypeBuffer &query, corecvs::RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K) = 0;
};


class DescriptorMatcherProviderImpl : public AlgoNaming
{
public:
    virtual DescriptorMatcher* getDescriptorMatcher(const DescriptorType &descriptor, const MatcherType &matcher, const std::string &params = "") = 0;
    virtual bool provides(const DescriptorType &descriptor, const MatcherType &matcher) = 0;

    virtual ~DescriptorMatcherProviderImpl() {}
};

class DescriptorMatcherProvider : public AlgoCollectionNaming<DescriptorMatcherProviderImpl>
{
public:
    DescriptorMatcher* getMatcher(const DescriptorType &descriptor, const MatcherType &matcher, const std::string &params = "");

    static DescriptorMatcherProvider& getInstance();
    ~DescriptorMatcherProvider();


private:
    DescriptorMatcherProvider();
    DescriptorMatcherProvider(const DescriptorMatcherProvider&);
    DescriptorMatcherProvider& operator=(const DescriptorMatcherProvider&);
};
