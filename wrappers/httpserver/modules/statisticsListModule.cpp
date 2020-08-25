#include "statisticsListModule.h"
#include "statisticsContent.h"

#include "core/utils/utils.h"
#include <httpUtils.h>

using namespace corecvs;

bool StatisticsListModule::shouldProcessURL(std::string url)
{
    std::string path = url;
    if (HelperUtils::startsWith(path, "/stat")) {
        return true;
    }
    return false;
}

bool StatisticsListModule::shouldWrapURL(std::string /*url*/)
{
    return true;
}

std::shared_ptr<HttpContent> StatisticsListModule::getContentByUrl(std::string url)
{
    std::string urlPath = url;
    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    if (HelperUtils::startsWith(urlPath, "/stat"))
    {
        return std::shared_ptr<HttpContent>(new UnitedStatisticsContent(mStatisticsDAO->getCollector()));
    }

    return std::shared_ptr<HttpContent>();

}

StatisticsListModule::StatisticsListModule()
{
}
