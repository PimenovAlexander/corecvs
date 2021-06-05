#include "core/utils/utils.h"

#include "reflectionListModule.h"
#include "contents/reflectionContent.h"
#include "contents/jsonContent.h"

#include <httpUtils.h>
//#include "contents/wrapperContent.h"

using namespace corecvs;

bool ReflectionListModule::shouldProcessURL(const std::string& url)
{
    SYNC_PRINT(("ReflectionListModule::shouldProcessURL(%s): called\n", url.c_str()));
    return (url == "/reflections" ||
        HelperUtils::startsWith(url, "/reflection") ||
        HelperUtils::startsWith(url, "/ureflection"));
}

bool ReflectionListModule::shouldWrapURL(const std::string& url)
{
    return (url == "/reflections");
}

std::shared_ptr<HttpContent> ReflectionListModule::getContentByUrl(const std::string& url)
{
    SYNC_PRINT(("ReflectionListModule::getContentByUrl(%s): called\n", url.c_str()));

    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(url);

    if (url == "/reflections")
    {
        return std::shared_ptr<HttpContent>(new ReflectionListContent(mReflectionsDAO->getReflectionNames()));
    }

    if (HelperUtils::startsWith(url, "/reflection") || HelperUtils::startsWith(url, "/ureflection"))
    {
        std::string reflectionName = "Main";
        if (!query.empty() && query.at(0).first == "name")
        {
            reflectionName = query.at(0).second;
        }

        LockableObject *object = mReflectionsDAO->getReflectionObject(reflectionName);

        if (object == nullptr) {
            return std::shared_ptr<HttpContent>(new ReflectionContent(nullptr));
        }

        if (query.size() > 1)
        {
            auto *content = new JSONContent();
            std::string newValue;
            bool result = ReflectionContent(object).changeValue(url, newValue);
            content->setResult(result);
            content->setValue(newValue);
            return std::shared_ptr<HttpContent>(content);
        }

        if (HelperUtils::startsWith(url, "/reflection")) {
            return std::shared_ptr<HttpContent>(new ReflectionContent(object));
        } else {
            return std::shared_ptr<HttpContent>(new ReflectionContent(object));
        }
    }
    return std::shared_ptr<HttpContent>(nullptr);
}

ReflectionListModule::ReflectionListModule()
{
}
