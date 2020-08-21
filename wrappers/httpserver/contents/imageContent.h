#ifndef IMAGE_CONTENT_H_
#define IMAGE_CONTENT_H_

#include <memory>
#include <vector>
#include <string>

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "httpContent.h"


class ImageListContent : public HttpContent
{
public:
    ImageListContent(std::vector<std::string> names);
    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType();

private:
    std::vector<std::string> mNames;
};

class ImageContent : public HttpContent
{
public:
    ImageContent(std::shared_ptr<corecvs::RGB24Buffer> image, double scale = 1.0, std::string format = "jpg");
    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType();

private:
    double mScale;
    std::string mFormat;
    std::shared_ptr<corecvs::RGB24Buffer> mImage;
};

#endif
