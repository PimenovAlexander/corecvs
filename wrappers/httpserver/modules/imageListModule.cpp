#include "contents/jsonContent.h"
#include "contents/imageContent.h"
#include "imageListModule.h"



ImageListModule::ImageListModule()
{
}


bool ImageListModule::shouldProcessURL(QUrl url)
{
    QString path = url.path();
    if (path.startsWith("/framelist")) {
        return true;
    }

    if (path.startsWith("/imagelist.json")) {
        return true;
    }

    if (path.startsWith("/frame.jpg") || path.startsWith("/frame.bmp") || path.startsWith("/frame.png")) {
        return true;
    }
    return false;
}

bool ImageListModule::shouldWrapURL(QUrl url)
{
    QString path = url.path();
    if (path.startsWith("/framelist")) {
        return true;
    }

    return false;
}


QSharedPointer<HttpContent> ImageListModule::getContentByUrl(QUrl url)
{
    QString urlPath = url.path();
    QList<QPair<QString, QString> > query = QUrlQuery(url).queryItems();

    if (urlPath.startsWith("/framelist"))
    {
        return QSharedPointer<HttpContent>(new ImageListContent(mImages->getImageNames()));
    }
    if (urlPath.startsWith("/frame.jpg") ||
        urlPath.startsWith("/frame.bmp") ||
        urlPath.startsWith("/frame.png"))
    {
        QString imageName = "Main";
        if (query.size() > 0 && query.at(0).first == "name")
        {
            imageName = query.at(0).second;
        }

        double scale = 1.0;
        if (query.size() > 1 && query.at(1).first == "scale")
        {
            bool ok = true;
            scale = query.at(1).second.toDouble(&ok) / 100.0;
            if (!ok || scale <= 0.0 || scale >= 3.0)
                scale = 1.0;
        }

        QString format = "JPG";
        if(urlPath.startsWith("/frame.bmp")) {
            format = "BMP";
        }
        if(urlPath.startsWith("/frame.png")) {
            format = "PNG";
        }

        MetaImage image = mImages->getImage(imageName);
        if (image.mImage.isNull())
        {
            return QSharedPointer<HttpContent>(new ImageContent(QSharedPointer<QImage>(NULL)));
        }

        return QSharedPointer<HttpContent>(new ImageContent(image.mImage, scale, format));
    }

    if (urlPath.startsWith("/imagelist.json"))
    {
        QString prefix = "";
        if (query.size() > 0 && query.at(0).first == "name")
        {
            prefix = query.at(0).second;
        }

        uint64_t since = 0;
        if (query.size() > 1 && query.at(1).first == "since")
        {
            since = query.at(1).second.toULongLong();
        }

        QVariantMap vars;

        QList<QString> names = mImages->getImageNames();
        for (int i = 0; i < names.size(); i++)
        {
            if (!names[i].startsWith(prefix))
                continue;
            MetaImage image = mImages->getImage(names[i]);
            if (image.mTimestamp <= since)
                continue;
            vars[names[i]] = QVariant((qlonglong)image.mTimestamp);
        }

        return QSharedPointer<HttpContent>(new JSONContent(vars));
    }

    return QSharedPointer<HttpContent>();
}



