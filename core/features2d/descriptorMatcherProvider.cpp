#include "core/features2d/descriptorMatcherProvider.h"

#include "core/utils/global.h"

DescriptorMatcher* DescriptorMatcherProvider::getMatcher(const DescriptorType &type, const MatcherType &matcher)
{
    for (std::vector<DescriptorMatcherProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        if ((*p)->provides(type, matcher))
        {
            return (*p)->getDescriptorMatcher(type, matcher);
		}
	}
    CORE_ASSERT_FAIL_P(("DescriptorMatcherProvider::getMatcher(%s, %s): no providers", type.c_str(), matcher.c_str()));
    return 0;
}

void DescriptorMatcher::knnMatch(RuntimeTypeBuffer &query, RuntimeTypeBuffer &train, std::vector<std::vector<RawMatch> > &matches, size_t K)
{
	knnMatchImpl(query, train, matches, K);
}

DescriptorMatcherProvider::~DescriptorMatcherProvider()
{
    for (std::vector<DescriptorMatcherProviderImpl*>::iterator p = providers.begin(); p != providers.end(); ++p)
    {
        delete *p;
    }
	providers.clear();
}

void DescriptorMatcherProvider::add(DescriptorMatcherProviderImpl *provider)
{
	providers.push_back(provider);
}

DescriptorMatcherProvider& DescriptorMatcherProvider::getInstance()
{
	static DescriptorMatcherProvider provider;
	return provider;
}

DescriptorMatcherProvider::DescriptorMatcherProvider()
{}
