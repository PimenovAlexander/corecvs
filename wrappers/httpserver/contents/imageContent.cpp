#include "imageContent.h"
#include "core/utils/utils.h"
#include "core/fileformats/bmpLoader.h"

using namespace corecvs;


#ifdef WITH_SWSCALE
#include "swScaler.h"
#endif

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef WITH_LIBGIF
#include "libgifFileReader.h"
#endif

using namespace corecvs;

ImageContent::ImageContent(std::shared_ptr<corecvs::RGB24Buffer> image, double scale, std::string format) :
    mScale(scale),
    mFormat(format),
    mImage(image)
{
}

std::vector<uint8_t> ImageContent::getContent()
{
    SYNC_PRINT(("ImageContent::getContent(): called\n"));
    if (!mImage) {
        SYNC_PRINT((" Image :is null\n"));
    } else {
        SYNC_PRINT((" Image :[%d x %d]\n", mImage->w, mImage->h));
    }
    SYNC_PRINT((" Format :%s\n", mFormat.c_str()));
    SYNC_PRINT((" Scale  :%lf\n", mScale));


    std::vector<uint8_t> data;
    if (mImage != NULL)
    {
        RGB24Buffer *toSend = mImage.get();
#ifdef WITH_SWSCALE
        if (mScale != 1.0) {
            toSend = SWScaler::scale(toSend, mScale);
        }
#else
        if (mScale == 0.5) {
            toSend = AbstractMipmapPyramid<RGB24Buffer>::downsample2(toSend);
        }
#endif

        do {
#if WITH_LIBJPEG
            if (mFormat == "JPG") {
                SYNC_PRINT(("Storing jpeg\n"));
                LibjpegFileReader().saveJPEG(data, toSend);
            }
#endif

#if WITH_LIBPNG
            if (mFormat == "PNG") {
                LibpngFileReader().savePNG(data, toSend);
            }
#endif

            if (mFormat == "BMP") {
                SYNC_PRINT(("Storing bmp\n"));
                BMPLoader().save(data, toSend);
            }
        } while (false);


        if (toSend != mImage.get()) {
            delete_safe(toSend);
        }
    } else {
        return std::vector<uint8_t>();
    }
    return data;
}

std::string ImageContent::getContentType()
{
    std::string mimeType = std::string("image/") + HelperUtils::toLower(mFormat);
    SYNC_PRINT(("ImageContent::getContentType(): %s\n", mimeType.c_str()));

    return mimeType;
}

ImageListContent::ImageListContent(std::vector<std::string> names) :
    mNames(names)
{

}

 std::vector<uint8_t> ImageListContent::getContent()
{
    std::ostringstream data;
    data << "<h1>Frames</h1>\n";
    data << "<ol>\n";
    for(std::string name : mNames)
    {
        data << "  <li><a href=\"frame.jpg?name=" << name << "\">";
        data << name;
        data << "</a>&nbsp";
        data << "<a href=\"frame.bmp?name=" << name << "\">bmp</a>&nbsp";
        data << "<a href=\"frame.png?name=" << name << "\">png</a>&nbsp";
        data << "<a href=\"frame.jpg?name=" << name << "&scale=30\">jpg 30%</a>&nbsp";
        data << "<a href=\"frame.bmp?name=" << name << "&scale=30\">bmp 30%</a>&nbsp";
        data << "<a href=\"frame.png?name=" << name << "&scale=30\">png 30%</a>&nbsp";
        data << "</li>\n";
    }

    data << "</ol>\n";

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::string ImageListContent::getContentType()
{
    return "text/html";
}
