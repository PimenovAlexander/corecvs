#include "resourcePackModule.h"

ResourcePackModule::ResourcePackModule(CompiledResourceDirectoryEntry *Data, int size) :
    size(size)
{
    for(int i = 0; i < size; i++)
    {
        data.insert(std::pair<std::string, CompiledResource>(
                Data->name,
                CompiledResource(Data->data, Data->length)));
        Data++;
    }
}

bool ResourcePackModule::shouldProcessURL(std::string url)
{
    std::string path = HttpUtils::getPath(url);
    SYNC_PRINT(("--- For URL <%s> Would search resource <%s>\n", url.c_str(), path.c_str()));

    return (data.find(path) != data.end());
}

bool ResourcePackModule::shouldWrapURL(std::string url)
{
    return false;
}

std::shared_ptr<HttpContent> ResourcePackModule::getContentByUrl(std::string url)
{
    std::string path = HttpUtils::getPath(url);
    return std::shared_ptr<HttpContent>(new ResourcePackContent(path, &data[path]));
}
