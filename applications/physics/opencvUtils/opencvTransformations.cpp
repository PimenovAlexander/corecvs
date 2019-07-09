#include "opencvTransformations.h"

#include "opencv2/core.hpp"
#include <QImage>


opencvTransformations::opencvTransformations()
{

}

QImage opencvTransformations::mat2RealQImage(cv::Mat const &src)     // B<->R
{
    //cv::Mat input(src.rows,src.cols,src.type()); // here we have problem
    //cv::Mat input = src.copyTo();
    //cv::imshow("mat",src);
    //QImage img = QImage((uchar*) input.data, input.cols, input.rows, input.step, QImage::Format_RGB888);
    QImage img = QImage((uchar*) src.data, src.cols, src.rows, src.step, QImage::Format_RGB888);
    QImage res = img.convertToFormat(QImage::Format_RGB32);
    return res.copy();
}

cv::Mat opencvTransformations::QImage2Mat(QImage const &srcc)
{
    QImage src = srcc.convertToFormat(QImage::Format_RGB888);
    cv::Mat matOutput = cv::Mat( src.height(),src.width(),CV_8UC3,const_cast<uchar*>(src.bits()),src.bytesPerLine());
    cv::Mat result;
    matOutput.copyTo(result);
    return result;
}
