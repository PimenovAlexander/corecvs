#ifndef IMAGELISTMODULE_H
#define IMAGELISTMODULE_H

#include <memory>

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#include "httpServerModule.h"

struct MetaImage {
    std::shared_ptr<RGB24Buffer> mImage;
    uint64_t mTimestamp;

    MetaImage(std::shared_ptr<RGB24Buffer> image = QSharedPointer<QImage>(), uint64_t timestamp = 0) :
        mImage(image),
        mTimestamp(timestamp)
    {}
};

class ImageListModuleDAO {
public:
    virtual QList<QString> getImageNames() = 0;
    virtual MetaImage      getImage(QString name) = 0;
};

class ImageListModuleHashDAO : public ImageListModuleDAO {
public:
    QHash<QString, QSharedPointer<QImage> > mImages;

    virtual QList<QString> getImageNames() {
        return mImages.keys();
    }

    virtual MetaImage getImage(QString name)
    {
        if (!mImages.contains(name)) {
            return MetaImage();
        }
        return MetaImage(mImages[name]);
    }
};


class ImageListModule : public HttpServerModule
{
public:
    ImageListModuleDAO *mImages;

    virtual bool shouldProcessURL(QUrl url);
    virtual bool shouldWrapURL(QUrl url);
    virtual QSharedPointer<HttpContent> getContentByUrl(QUrl url);

    ImageListModule();
};

#endif // IMAGELISTMODULE_H
