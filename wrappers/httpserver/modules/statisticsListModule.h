#ifndef STATISTICSLISTMODULE_H
#define STATISTICSLISTMODULE_H

#include "core/stats/calculationStats.h"
#include "modules/httpServerModule.h"

class StatisticsModuleDAO {
public:
    /*virtual BaseTimeStatisticsCollector &fetchCollector();
    virtual void relaseCollector();*/
    /* Get a stat for a while */
    virtual corecvs::BaseTimeStatisticsCollector getCollector() = 0;
};

class StatisticsListModule : public HttpServerModule
{
public:
    StatisticsModuleDAO *mStatisticsDAO;

    bool shouldProcessURL(std::string url) override;
    bool shouldWrapURL(std::string url) override;
    std::shared_ptr<HttpContent> getContentByUrl(std::string url) override;

    StatisticsListModule();
};

#endif // STATISTICSLISTMODULE_H
