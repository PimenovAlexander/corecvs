#include "core/utils/utils.h"

#include "reflectionListModule.h"
#include "contents/reflectionContent.h"
#include "contents/jsonContent.h"

#include <httpUtils.h>
//#include "contents/wrapperContent.h"

using namespace corecvs;

bool ReflectionListModule::shouldProcessURL(std::string url)
{
    SYNC_PRINT(("ReflectionListModule::shouldProcessURL(%s): called\n", url.c_str()));
    std::string urlPath = url;
    if (urlPath == "/reflections" ||
        HelperUtils::startsWith(urlPath, "/reflection") ||
        HelperUtils::startsWith(urlPath, "/ureflection"))
    {
        return true;
    }
    return false;
}

bool ReflectionListModule::shouldWrapURL(std::string url)
{
    std::string urlPath = url;
    if (urlPath == "/reflections")
    {
        return true;
    }
    return false;

}

std::shared_ptr<HttpContent> ReflectionListModule::getContentByUrl(std::string url)
{
    std::string urlPath = url;
    SYNC_PRINT(("ReflectionListModule::getContentByUrl(%s): called\n", urlPath.c_str()));

    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    if (urlPath == "/reflections")
    {
        return std::shared_ptr<HttpContent>(new ReflectionListContent(mReflectionsDAO->getReflectionNames()));
    }

    if (HelperUtils::startsWith(urlPath, "/reflection") || HelperUtils::startsWith(urlPath, "/ureflection"))
    {
        std::string reflectionName = "Main";
        if (!query.empty() && query.at(0).first == "name")
        {
            reflectionName = query.at(0).second;
        }

        LockableObject *object = mReflectionsDAO->getReflectionObject(reflectionName);

        if (object == NULL) {
            return std::shared_ptr<HttpContent>(new ReflectionContent(NULL));
        }

        if (query.size() > 1)
        {
            JSONContent *content = new JSONContent();
            std::string newValue;
            bool result = ReflectionContent(object).changeValue(url, newValue);
            content->setResult(result);
            content->setValue(newValue);
            return std::shared_ptr<HttpContent>(content);
        }

        if (HelperUtils::startsWith(urlPath, "/reflection")) {
            return std::shared_ptr<HttpContent>(new ReflectionContent(object));
        } else {
            return std::shared_ptr<HttpContent>(new ReflectionContent(object));
        }
    }
    return std::shared_ptr<HttpContent>(NULL);
}

ReflectionListModule::ReflectionListModule()
{
}
