
#ifndef CORECVS_MAVLINKMODULE_H
#define CORECVS_MAVLINKMODULE_H

#include "test/gcs/mavlink/common/mavlink.h"
#include "httpServerModule.h"

class MavLinkContent : public HttpContent
{
public:
    MavLinkContent() {}
    MavLinkContent(int currentMissionItem) {
        missionItem = currentMissionItem;
        containsResponse = true;
    }

    std::vector<uint8_t> getContent() override;
    std::string getContentType() override
    {
        return "text/html";
    }
private:
    // Required as a mock if no droneApp is present to acquire current mission item's index
    int missionItem = 0;
    bool containsResponse = false;
};

class MavLinkModule: public HttpServerModule {
public:
    bool shouldProcessURL(const std::string& url) override;
    bool shouldWrapURL   (const std::string& url) override;
    std::shared_ptr<HttpContent> getContentByUrl(const std::string& url) override;
    bool shouldPollURL(const std::string& url) override;

    MavLinkModule();
};

#endif //CORECVS_MAVLINKMODULE_H
