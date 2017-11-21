#include "core/features2d/descriptorExtractorProvider.h"

#include "core/utils/global.h"


DescriptorExtractor* DescriptorExtractorProvider::getDescriptorExtractor(const DescriptorType &type, const std::string &params)
{
    for (std::vector<DescriptorExtractorProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        if ((*p)->provides(type))
        {
            return (*p)->getDescriptorExtractor(type, params);
        }
    }
    CORE_ASSERT_FAIL_P(("DescriptorExtractorProvider::getDescriptorExtractor(%s): no providers", type.c_str()));
    return 0;
}


DescriptorExtractorProvider::~DescriptorExtractorProvider()
{
}

DescriptorExtractorProvider& DescriptorExtractorProvider::getInstance()
{
    static DescriptorExtractorProvider provider;
    return provider;
}

DescriptorExtractorProvider::DescriptorExtractorProvider()
{
}

void DescriptorExtractor::compute(corecvs::RuntimeTypeBuffer &image, std::vector<KeyPoint> &keyPoints, corecvs::RuntimeTypeBuffer &buffer, void* pRemapCache)
{
	computeImpl(image, keyPoints, buffer, pRemapCache);
}
