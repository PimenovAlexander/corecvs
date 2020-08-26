#include "resourcePackModule.h"

ResourcePackModule::ResourcePackModule(CompiledResourceDirectoryEntry *data, int size) :
    data(data),
    size(size)
{

}

bool ResourcePackModule::shouldProcessURL(std::string url)
{
    std::string path = HttpUtils::getPath(url);
    SYNC_PRINT(("--- For URL <%s> Would search resource <%s>\n", url.c_str(), path.c_str()));


    for (int i = 0; i < size; i++)
    {
        SYNC_PRINT(("-- Checking %s against %s\n", path.c_str(), data[i].name));
        if (path == data[i].name) {
            return true;
            SYNC_PRINT(("--- Match"));
        }
    }
    return false;
}

bool ResourcePackModule::shouldWrapURL(std::string url)
{
    return false;
}

std::shared_ptr<HttpContent> ResourcePackModule::getContentByUrl(std::string url)
{
    std::string path = HttpUtils::getPath(url);
    for (int i = 0; i < size; i++)
    {
        if (path == data[i].name) {
            return std::shared_ptr<HttpContent>(new ResourcePackContent(&data[i]));
        }
    }
    return std::shared_ptr<HttpContent>();

}
