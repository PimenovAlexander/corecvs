/**
 * \file OpenCVTools.h
 * \brief Add Comment Here
 *
 * \date Apr 6, 2011
 * \author alexander
 */
#ifndef OPENCV_TOOLS_H
#define OPENCV_TOOLS_H

#define __XMLDocument_FWD_DEFINED__  // to omit conflict "msxml.h:3376: using typedef-name 'XMLDocument' after 'class'"

#include <opencv2/core/core.hpp> // CvSize, IplImage

#include "utils/global.h"

#include "buffers/g8Buffer.h"
#include "math/vector/vector2d.h"
#include "buffers/g12Buffer.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "buffers/runtimeTypeBuffer.h"

using corecvs::G12Buffer;
using corecvs::G8Buffer;
using corecvs::RGB24Buffer;

#define CVMAT_FROM_IPLIMAGE( cvmat, iplimage, copydata ) cv::Mat cvmat = cv::cvarrToMat( iplimage, copydata )

class OpenCVTools
{
public:

    /* This stuff needs to be reimplemented in new way. With MultiConverter. With same logic as swScale */

    static IplImage *getCVImageFromRGB24Buffer(RGB24Buffer *input);
    static IplImage *getCVImageFromG12Buffer  (G12Buffer *input);
    static IplImage *getCVImageFromG8Buffer   (G8Buffer *input);

    static cv::Mat getCVMatFromRGB24Buffer(RGB24Buffer *input);

    static RGB24Buffer *getRGB24BufferFromCVImage(IplImage *input);
    static G12Buffer   *getG12BufferFromCVImage  (IplImage *input);
    static G8Buffer    *getG8BufferFromCVImage   (IplImage *input);

    static RGB24Buffer *getRGB24BufferFromCVMat(const cv::Mat &input);
    static G8Buffer    *getG8BufferFromCVMat   (const cv::Mat &input);

    static cv::Mat convert(const corecvs::RuntimeTypeBuffer &buffer);
    static corecvs::RuntimeTypeBuffer convert(const cv::Mat &mat);

template<typename OtherStruct>
    static CvSize getCvSizeFromVector(const OtherStruct &other)
    {
        return cvSize(other.x(), other.y());
    }
};

class CV2Core {
public:
    static corecvs::Vector2dd Vector2ddFromPoint2f(cv::Point2f &input) {
        return corecvs::Vector2dd(input.x, input.y);
    }
};

#endif // OPENCV_TOOLS_H
