
#include <httpUtils.h>
#include "mavlinkmodule.h"

using namespace corecvs;

bool MavLinkModule::shouldProcessURL(const std::string& url) {
    return HelperUtils::startsWith(url, "/");
}

bool MavLinkModule::shouldPollURL(const std::string& url)
{
    return false;
}

bool MavLinkModule::shouldWrapURL(const std::string& url)
{
    return HelperUtils::startsWith(url, "/mavlink");
}

std::shared_ptr<HttpContent> MavLinkModule::getContentByUrl(const std::string& url)
{
    std::string urlPath(url);

    // Removes poll prefix
    checkAndRewritePollPrefix(urlPath);

    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    std::cout << "MavLink Module : " << urlPath << std::endl;

    if (HelperUtils::startsWith(urlPath, "/missionItem"))
    {
        std::cout << "Mission Item" << std::endl;
        return std::shared_ptr<HttpContent>();
    }
    if (HelperUtils::startsWith(urlPath, "/updateMission"))
    {
        std::cout << "Update Mission" << std::endl;
        return std::shared_ptr<HttpContent>();
    }

    return std::shared_ptr<HttpContent>();
}

MavLinkModule::MavLinkModule() {

}
