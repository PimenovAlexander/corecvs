#include "core/utils/utils.h"

#include "reflectionListModule.h"
#include "contents/reflectionContent.h"
#include "contents/jsonContent.h"
//#include "contents/wrapperContent.h"

using namespace corecvs;

bool ReflectionListModule::shouldProcessURL(std::string url)
{
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

#if 0
    QList<QPair<QString, QString> > query = QUrlQuery(url).queryItems();

    if (urlPath == "/reflections")
    {
        return QSharedPointer<HttpContent>(new ReflectionListContent(mReflectionsDAO->getReflectionNames()));
    }

    if (urlPath.startsWith("/reflection") || urlPath.startsWith("/ureflection"))
    {
        QString reflectionName = "Main";
        if (!query.empty() && query.at(0).first == "name")
        {
            reflectionName = query.at(0).second;
        }

        LockableObject *object = mReflectionsDAO->getReflectionObject(reflectionName);

        if (object == NULL) {
            return QSharedPointer<HttpContent>(new WrapperContent(new ReflectionContent(NULL)));
        }

        if (query.size() > 1)
        {
            JSONContent *content = new JSONContent();
            QVariant newValue;
            bool result = ReflectionContent(object).changeValue(url, newValue);
            content->setResult(result);
            content->setValue(newValue);
            return QSharedPointer<HttpContent>(content);
        }

        if (HelperUtils::startsWith(urlPath, "/reflection")) {
            return WrapperContent::Wrap (std::shared_ptr<HttpContent>(new ReflectionContent(object)));
        } else {
            return WrapperContent::WrapS(std::shared_ptr<HttpContent>(new ReflectionContent(object)));
        }
    }
#endif
    return std::shared_ptr<HttpContent>(NULL);
}

ReflectionListModule::ReflectionListModule()
{
}
