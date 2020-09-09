#include "statisticsListModule.h"
#include "statisticsContent.h"

#include "core/utils/utils.h"
#include <httpUtils.h>

using namespace corecvs;

bool StatisticsListModule::shouldProcessURL(const std::string& url)
{
    if (HelperUtils::startsWith(url, "/stat")) {
        return true;
    }
    return false;
}

bool StatisticsListModule::shouldWrapURL(const std::string& /*url*/)
{
    return true;
}

std::shared_ptr<HttpContent> StatisticsListModule::getContentByUrl(const std::string& url)
{
    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(url);

    if (HelperUtils::startsWith(url, "/stat"))
    {
        return std::shared_ptr<HttpContent>(new UnitedStatisticsContent(mStatisticsDAO->getCollector()));
    }

    return std::shared_ptr<HttpContent>();

}

StatisticsListModule::StatisticsListModule()
{
}
