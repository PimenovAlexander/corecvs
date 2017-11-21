#pragma once

#include "core/features2d/imageKeyPoints.h"
#include "core/features2d/algoBase.h"


class DescriptorExtractor : public virtual AlgoBase
{
public:
    void compute(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descriptors, void* pRemapCache);
    virtual ~DescriptorExtractor() {}
protected:
    virtual void computeImpl(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &descriptors, void* pRemapCache) = 0;
};

class DescriptorExtractorProviderImpl : public AlgoNaming
{
public:
    virtual bool provides(const DescriptorType &type) = 0;
    virtual DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type, const std::string &params = "") = 0;
    virtual ~DescriptorExtractorProviderImpl() {}
};

class DescriptorExtractorProvider : public AlgoCollectionNaming<DescriptorExtractorProviderImpl>
{
public:
    DescriptorExtractor* getDescriptorExtractor(const DescriptorType &type, const std::string &params = "");

    static DescriptorExtractorProvider& getInstance();
    virtual ~DescriptorExtractorProvider();

private:
    DescriptorExtractorProvider();
    DescriptorExtractorProvider(const DescriptorExtractorProvider&);
    DescriptorExtractorProvider& operator=(const DescriptorExtractorProvider&);
};
