#include "core/features2d/descriptorMatcherProvider.h"

#include "core/utils/global.h"


using namespace corecvs;

DescriptorMatcher* DescriptorMatcherProvider::getMatcher(const DescriptorType &type, const MatcherType &matcher, const std::string &params)
{
    for (std::vector<DescriptorMatcherProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        if ((*p)->provides(type, matcher))
        {
            return (*p)->getDescriptorMatcher(type, matcher, params);
        }
    }

    try
	{
		CORE_ASSERT_FAIL_P(("DescriptorMatcherProvider::getMatcher(%s, %s): no providers", type.c_str(), matcher.c_str()));	
	}
    catch (AssertException)
	{
	
	}

    return 0;
}

void DescriptorMatcher::knnMatch(RuntimeTypeBuffer &query, RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K)
{
    knnMatchImpl(query, train, matches, K);
}

DescriptorMatcherProvider::~DescriptorMatcherProvider()
{

}

DescriptorMatcherProvider& DescriptorMatcherProvider::getInstance()
{
    static DescriptorMatcherProvider provider;
    return provider;
}

DescriptorMatcherProvider::DescriptorMatcherProvider()
{}
