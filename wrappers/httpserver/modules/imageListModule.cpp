#include "core/utils/utils.h"

#include "contents/jsonContent.h"
#include "contents/imageContent.h"
#include "imageListModule.h"

#include <httpUtils.h>

using namespace corecvs;

ImageListModule::ImageListModule()
{
}


bool ImageListModule::shouldProcessURL(const std::string& url)
{
    string urlPath(url);
    if (checkAndRewritePollPrefix(urlPath)) {
        return shouldPollURL(urlPath);
    }
    return (
            HelperUtils::startsWith(url, "/framelist") ||
            HelperUtils::startsWith(url, "/imagelist.json") ||
            HelperUtils::startsWith(url, "/frame.jpg") ||
            HelperUtils::startsWith(url, "/frame.bmp") ||
            HelperUtils::startsWith(url, "/frame.png"));
}

bool ImageListModule::shouldPollURL(const std::string& url)
{
    if (HelperUtils::startsWith(url, "/cameraImage"))
        return true;
    return false;
}

bool ImageListModule::shouldWrapURL(const std::string& url)
{
    return HelperUtils::startsWith(url, "/framelist");
}


std::shared_ptr<HttpContent> ImageListModule::getContentByUrl(const std::string& url)
{
    string urlPath(url);

    // Removes poll prefix
    checkAndRewritePollPrefix(urlPath);

    std::vector<std::pair<std::string, std::string> > query = HttpUtils::parseParameters(urlPath);

    std::cout << "Image List Module : " << urlPath << std::endl;

    if (HelperUtils::startsWith(urlPath, "/framelist"))
    {
        return std::shared_ptr<HttpContent>(new ImageListContent(mImages->getImageNames()));
    }
    if (HelperUtils::startsWith(urlPath, "/frame.jpg") ||
        HelperUtils::startsWith(urlPath, "/frame.bmp") ||
        HelperUtils::startsWith(urlPath, "/frame.png"))
    {
        std::string imageName = "Main";
        if (!query.empty() && query.at(0).first == "name")
        {
            imageName = query.at(0).second;
        }

        double scale = 1.0;
        if (query.size() > 1 && query.at(1).first == "scale")
        {
            std::string strScale = query.at(1).second;
            scale = HelperUtils::parseDouble(strScale) / 100.0;
            if (scale <= 0.0 || scale >= 3.0)
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
            return std::shared_ptr<HttpContent>(new ImageContent(std::shared_ptr<corecvs::RGB24Buffer>(nullptr)));
        }

        return std::shared_ptr<HttpContent>(new ImageContent(image.mImage, scale, format));
    }

    if (HelperUtils::startsWith(urlPath,"/imagelist.json"))
    {
        std::string prefix;
        if (!query.empty() && query.at(0).first == "name")
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

        for(const std::string& imageName: names)
        {
            if (!HelperUtils::startsWith(imageName, prefix))
                continue;
            MetaImage image = mImages->getImage(imageName);
            if (image.mTimestamp <= since)
                continue;
        }

        // Empty pointer (TBD)
        return std::shared_ptr<HttpContent>();
    }

    return std::shared_ptr<HttpContent>();
}



