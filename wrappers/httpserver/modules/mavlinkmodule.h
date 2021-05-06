
#ifndef CORECVS_MAVLINKMODULE_H
#define CORECVS_MAVLINKMODULE_H

#include "test/gcs/mavlink/common/mavlink.h"
#include "httpServerModule.h"

class MavLinkModule: public HttpServerModule {
public:
    bool shouldProcessURL(const std::string& url) override;
    bool shouldWrapURL   (const std::string& url) override;
    std::shared_ptr<HttpContent> getContentByUrl(const std::string& url) override;
    bool shouldPollURL(const std::string& url) override;

    MavLinkModule();
};

#endif //CORECVS_MAVLINKMODULE_H
