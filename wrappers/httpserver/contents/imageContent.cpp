#include "imageContent.h"
#include "core/utils/utils.h"

using namespace corecvs;

ImageContent::ImageContent(std::shared_ptr<corecvs::RGB24Buffer> image, double scale, std::string format) :
    mScale(scale),
    mFormat(format),
    mImage(image)
{
}

std::vector<uint8_t> ImageContent::getContent()
{
    std::vector<uint8_t> data;
    if (mImage != NULL)
    {

    } else {

    }
    return data;
}

std::string ImageContent::getContentType()
{    
    return std::string("image/") + HelperUtils::toLower(mFormat);
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
        data << "<a href=\"frame.png?name=" << name << "\">png</a>";
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
