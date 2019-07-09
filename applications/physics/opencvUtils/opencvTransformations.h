#ifndef OPENCVTRANSFORMATIONS_H
#define OPENCVTRANSFORMATIONS_H

#include "opencv2/core.hpp"
#include <QImage>

class opencvTransformations
{
public:
    opencvTransformations();
    QImage mat2RealQImage(const cv::Mat &src);
    cv::Mat QImage2Mat(const QImage &srcc);
};

#endif // OPENCVTRANSFORMATIONS_H
