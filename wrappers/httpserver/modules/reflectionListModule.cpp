#include "reflectionListModule.h"
#include "contents/reflectionContent.h"
#include "contents/jsonContent.h"
//#include "contents/wrapperContent.h"

using namespace corecvs;

bool ReflectionListModule::shouldProcessURL(QUrl url)
{
    QString urlPath = url.path();
    if (urlPath == "/reflections" ||
        urlPath.startsWith("/reflection") ||
        urlPath.startsWith("/ureflection"))
    {
        return true;
    }
    return false;
}

bool ReflectionListModule::shouldWrapURL(std::string url)
{
    QString urlPath = url.path();
    if (urlPath == "/reflections")
    {
        return true;
    }
    return false;

}

QSharedPointer<HttpContent> ReflectionListModule::getContentByUrl(QUrl url)
{
    QString urlPath = url.path();
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

        if (urlPath.startsWith("/reflection")) {
            return WrapperContent::Wrap (QSharedPointer<HttpContent>(new ReflectionContent(object)));
        } else {
            return WrapperContent::WrapS(QSharedPointer<HttpContent>(new ReflectionContent(object)));
        }
    }
    return QSharedPointer<HttpContent>(NULL);
}

ReflectionListModule::ReflectionListModule()
{
}
