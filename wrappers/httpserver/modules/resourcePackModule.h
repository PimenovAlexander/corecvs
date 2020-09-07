#ifndef RESOURCEPACKMODULE_H
#define RESOURCEPACKMODULE_H

#include <httpUtils.h>
#include <memory.h>

#include "httpServerModule.h"
#include "compiledResourceDirectory.h"
#include "compiledResource.h"

#include <map>

class ResourcePackContent : public HttpContent
{
public:
    std::string name;
    CompiledResource *data;

    ResourcePackContent(std::string name, CompiledResource *data) :
        name(name),
        data(data)
    {}

    virtual std::vector<uint8_t> getContent() override
    {
        std::vector<uint8_t> result;
        result.resize(data->length);
        memcpy(result.data(), data->data, data->length);
        return result;
    }

    virtual std::string getContentType() override
    {
        return HttpUtils::extentionToMIME(name);
    }
};

class ResourcePackModule : public HttpServerModule
{
public:
    std::map<std::string, CompiledResource> data;
    int size;

    virtual bool shouldProcessURL(std::string url);
    virtual bool shouldWrapURL(std::string url);
    virtual std::shared_ptr<HttpContent> getContentByUrl(std::string url);


    ResourcePackModule(CompiledResourceDirectoryEntry *Data, int size);
};

#endif // RESOURCEPACKMODULE_H
