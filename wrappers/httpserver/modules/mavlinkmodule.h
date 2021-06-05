
#ifndef CORECVS_MAVLINKMODULE_H
#define CORECVS_MAVLINKMODULE_H

#include "test/gcs/mavlink/common/mavlink.h"
#include "httpServerModule.h"

class MavLinkContent : public HttpContent
{
public:
    MavLinkContent() {}
    MavLinkContent(int item) {
        value = item;
        containsValue = true;
    }
    MavLinkContent(const int items[]) {
        for(int i = 0; i < 3; i++) {
            values[i] = items[i];
        }
        containsValues = true;
    }

    std::vector<uint8_t> getContent() override;
    std::string getContentType() override
    {
        return "text/html";
    }
private:
    // Required as a mock if no droneApp is present to acquire current mission item's index and properties
    int value = 0;
    int values[3] = { 0, 0, 0 };
    bool containsValue = false;
    bool containsValues = false;
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
