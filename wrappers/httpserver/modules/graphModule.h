#ifndef GRAPHMODULE_H
#define GRAPHMODULE_H

#include "core/stats/graphData.h"
#include "httpServerModule.h"

#include "httpContent.h"

class GraphModuleDAO {
public:
    virtual void lockGraphData() = 0;
    virtual void unlockGraphData() = 0;

    virtual GraphData *getGraphData() = 0;
};


class GraphContent : public HttpContent
{
public:
    GraphContent(GraphModuleDAO *dao) :
        mDao(dao)
    {}

    std::vector<uint8_t> getContent() override;
    std::string getContentType() override
    {
        return "application/json";
    }

    GraphModuleDAO *mDao;
};




class GraphModule : public HttpServerModule
{
public:
    GraphModuleDAO *mGraphData;

    bool shouldProcessURL(const std::string& url) override;
    bool shouldWrapURL(const std::string& url) override;
    std::shared_ptr<HttpContent> getContentByUrl(const std::string& url) override;

    GraphModule();
};

#endif // GRAPHMODULE_H
