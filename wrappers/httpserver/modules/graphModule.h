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

    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType()
    {
        return "application/json";
    }

    GraphModuleDAO *mDao;
};




class GraphModule : public HttpServerModule
{
public:
    GraphModuleDAO *mGraphData;

    virtual bool shouldProcessURL(QUrl url);
    virtual bool shouldWrapURL(QUrl url);
    virtual QSharedPointer<HttpContent> getContentByUrl(QUrl url);

    GraphModule();
};

#endif // GRAPHMODULE_H
