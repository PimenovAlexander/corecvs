/**
 * \file OpenCVTools.h
 * \brief Add Comment Here
 *
 * \date Apr 6, 2011
 * \author alexander
 */

#ifndef OPENCVTOOLS_H_
#define OPENCVTOOLS_H_

#define __XMLDocument_FWD_DEFINED__  // to omit conflict "msxml.h:3376: using typedef-name 'XMLDocument' after 'class'"

#include <opencv/cv.h>

#include "global.h"

#include "g8Buffer.h"
#include "g12Buffer.h"
#include "rgb24Buffer.h"

using corecvs::G12Buffer;
using corecvs::G8Buffer;
using corecvs::RGB24Buffer;

class OpenCVTools
{
public:
    static IplImage *getCVImageFromRGB24Buffer(RGB24Buffer *input);
    static IplImage *getCVImageFromG12Buffer(G12Buffer *input);
    static IplImage *getCVImageFromG8Buffer(G8Buffer *input);

    static RGB24Buffer *getRGB24BufferFromCVImage(IplImage *input);
    static G12Buffer   *getG12BufferFromCVImage(IplImage *input);


template<typename OtherStruct>
    static CvSize getCvSizeFromVector(const OtherStruct &other)
    {
        return cvSize(other.x(), other.y());
    }


};

#endif /* OPENCVTOOLS_H_ */
