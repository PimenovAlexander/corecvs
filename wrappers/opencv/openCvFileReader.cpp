#include "openCvFileReader.h"
#include "openCvKeyPointsWrapper.h"
#include "OpenCVTools.h"

#include "global.h"

#include <exception>
#include <sstream>

#include <opencv2/core/core.hpp>        // Mat
#include <opencv2/highgui/highgui.hpp>  // imread

bool OpenCvBufferReaderProvider::provides(const std::string &filename)
{
    // TODO: make list of supported formats
    CORE_UNUSED(filename);
    return true;
}

BufferReader* OpenCvBufferReaderProvider::getBufferReader(const std::string &filename)
{
    CORE_UNUSED(filename);
    return new OpenCvBufferReader();
}

void init_opencv_reader_provider()
{
    BufferReaderProvider::getInstance().add(new OpenCvBufferReaderProvider);
}

corecvs::RGB24Buffer OpenCvBufferReader::readRgb(const std::string &s)
{
    cv::Mat img = cv::imread(s, CV_LOAD_IMAGE_COLOR);
    if (!(img.rows && img.cols && img.data))
    {
        std::stringstream ss;
        ss << "\"" << s << "\" is not a valid image file";
        throw std::invalid_argument(ss.str());
    }
    IplImage ip(img);
    auto* b = OpenCVTools::getRGB24BufferFromCVImage(&ip);
    corecvs::RGB24Buffer buffer = *b;
    delete b;
    return  buffer;
}

RuntimeTypeBuffer OpenCvBufferReader::read(const std::string &s)
{
    cv::Mat img = cv::imread(s, CV_LOAD_IMAGE_GRAYSCALE);
    if (!(img.rows && img.cols && img.data))
    {
        std::stringstream ss;
        ss << "\"" << s << "\" is not a valid image file";
        throw std::invalid_argument(ss.str());
    }
    return convert(img);
}

void OpenCvBufferReader::writeRgb(const corecvs::RGB24Buffer &buffer, const std::string &s)
{
    auto* b = OpenCVTools::getCVImageFromRGB24Buffer(&const_cast<corecvs::RGB24Buffer&>(buffer));
    cv::Mat mat(b);
    imwrite(s, mat);
    cvReleaseImage(&b);
}

void OpenCvBufferReader::write(const RuntimeTypeBuffer &buffer, const std::string &s)
{
    cv::Mat img = convert(buffer);
    imwrite(s, img);
}
