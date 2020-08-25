#include "core/utils/utils.h"

#include "contents/jsonContent.h"
#include "contents/imageContent.h"
#include "imageListModule.h"

#include <httpUtils.h>

using namespace corecvs;

ImageListModule::ImageListModule()
{
}


bool ImageListModule::shouldProcessURL(std::string url)
{
    std::string path = url;
    if (HelperUtils::startsWith(path, "/framelist")) {
        return true;
    }

    if (HelperUtils::startsWith(path, "/imagelist.json")) {
        return true;
    }

    if (HelperUtils::startsWith(path, "/frame.jpg") ||
        HelperUtils::startsWith(path, "/frame.bmp") ||
        HelperUtils::startsWith(path, "/frame.png"))
    {
        return true;
    }
    return false;
}

bool ImageListModule::shouldWrapURL(std::string url)
{
    std::string path = url;
    if (HelperUtils::startsWith(path, "/framelist")) {
        return true;
    }

    return false;
}


std::shared_ptr<HttpContent> ImageListModule::getContentByUrl(std::string url)
{
    std::string urlPath = url;
    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    if (HelperUtils::startsWith(urlPath, "/framelist"))
    {
        return std::shared_ptr<HttpContent>(new ImageListContent(mImages->getImageNames()));
    }
    if (HelperUtils::startsWith(urlPath, "/frame.jpg") ||
        HelperUtils::startsWith(urlPath, "/frame.bmp") ||
        HelperUtils::startsWith(urlPath, "/frame.png"))
    {
        std::string imageName = "Main";
        if (query.size() > 0 && query.at(0).first == "name")
        {
            imageName = query.at(0).second;
        }

        double scale = 1.0;
        if (query.size() > 1 && query.at(1).first == "scale")
        {
            bool ok = true;
            std::string strScale = query.at(1).second;
            scale = HelperUtils::parseDouble(strScale) / 100.0;
            if (!ok || scale <= 0.0 || scale >= 3.0)
                scale = 1.0;
        }

        std::string format = "JPG";
        if(HelperUtils::startsWith(urlPath, "/frame.bmp")) {
            format = "BMP";
        }
        if(HelperUtils::startsWith(urlPath, "/frame.png")) {
            format = "PNG";
        }

        MetaImage image = mImages->getImage(imageName);
        if (!image.mImage)
        {
            return std::shared_ptr<HttpContent>(new ImageContent(std::shared_ptr<corecvs::RGB24Buffer>(NULL)));
        }

        return std::shared_ptr<HttpContent>(new ImageContent(image.mImage, scale, format));
    }

    if (HelperUtils::startsWith(urlPath,"/imagelist.json"))
    {
        std::string prefix = "";
        if (query.size() > 0 && query.at(0).first == "name")
        {
            prefix = query.at(0).second;
        }

        uint64_t since = 0;
        if (query.size() > 1 && query.at(1).first == "since")
        {
            std::string strStamp = query.at(1).second;
            since = stol(strStamp);
        }

        std::vector<std::string> names = mImages->getImageNames();
        for (size_t i = 0; i < names.size(); i++)
        {
            if (!HelperUtils::startsWith(names[i], prefix))
                continue;
            MetaImage image = mImages->getImage(names[i]);
            if (image.mTimestamp <= since)
                continue;
            //vars[names[i]] = QVariant((qlonglong)image.mTimestamp);
        }

        return std::shared_ptr<HttpContent>();
    }

    return std::shared_ptr<HttpContent>();
}



