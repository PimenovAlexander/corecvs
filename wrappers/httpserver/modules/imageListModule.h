#ifndef IMAGELISTMODULE_H
#define IMAGELISTMODULE_H

#include <memory>
#include <vector>

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

#include "httpServerModule.h"

struct MetaImage {
    std::shared_ptr<corecvs::RGB24Buffer> mImage;
    uint64_t mTimestamp;

    MetaImage(std::shared_ptr<corecvs::RGB24Buffer> image = std::shared_ptr<corecvs::RGB24Buffer>(), uint64_t timestamp = 0) :
        mImage(image),
        mTimestamp(timestamp)
    {}
};

class ImageListModuleDAO {
public:
    virtual std::vector<std::string> getImageNames() = 0;
    virtual MetaImage                getImage(std::string name) = 0;
};

class ImageListModuleHashDAO : public ImageListModuleDAO {
public:
    std::map<std::string, MetaImage> mImages;

    virtual std::vector<std::string> getImageNames() {
        vector<std::string> names;
        names.reserve(mImages.size());
        for (auto it : mImages) {
            names.push_back(it.first);
        }
        return names;
    }

    virtual MetaImage getImage(std::string name)
    {
        if (mImages.count(name) == 0) {
            return MetaImage();
        }
        return MetaImage(mImages[name]);
    }
};


class ImageListModule : public HttpServerModule
{
public:
    ImageListModuleDAO *mImages;

    virtual bool shouldProcessURL(std::string url);
    virtual bool shouldWrapURL   (std::string url);
    virtual std::shared_ptr<HttpContent> getContentByUrl(std::string url);

    ImageListModule();
};

#endif // IMAGELISTMODULE_H
