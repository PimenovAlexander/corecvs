#include "resourcePackModule.h"

ResourcePackModule::ResourcePackModule(CompiledResourceDirectoryEntry *data, int size) :
    data(data),
    size(size)
{

}

bool ResourcePackModule::shouldProcessURL(std::string url)
{
    for (int i = 0; i < size; i++)
    {
        SYNC_PRINT(("-- Checking %s against %s\n", url.c_str(), data[i].name));
        if (url == data[i].name) {
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
    for (int i = 0; i < size; i++)
    {
        if (url == data[i].name) {
            return std::shared_ptr<HttpContent>(new ResourcePackContent(data));
        }
    }
    return std::shared_ptr<HttpContent>();

}
