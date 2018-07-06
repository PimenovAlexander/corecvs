#pragma once
#include "core/buffers/displacementBuffer.h"
#include "core/xml/generated/interpolationType.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs {

/**
 *  This class is caching the transformation of the buffer.
 *  There is a number of ways to store transformation for the buffer.
 *
 *  Typically you use these to search for a cached inverse map
 *  Your options are
 *    - DisplacementBuffer - stores small floating points offsets
 *
 *    - DirectRemapper -  it stores direct map in integer corrdiats
 *
 *    - FixedPointRemapper - this stores fixed point presenation of the mapping for every pixel
 *
 *
 *
 **/
class TransformationCache
{
public:
    /**
     * \param matrix     inverse matrix of the transformation applied
     * \param w          output width of the image
     * \param h          output height of the image
     * \param inputSize  size of input image
     *
     **/
    TransformationCache(Matrix33 matrix, int w, int h, Vector2d<int> inputSize);

    G12Buffer   *doDeformation(InterpolationType::InterpolationType type, G12Buffer *buffer);
    RGB24Buffer *doDeformation(InterpolationType::InterpolationType type, RGB24Buffer *inputFrame);

    ~TransformationCache();

private:
    DirectRemapper *mRemap;
    DisplacementBuffer *mDisplace;
    FixedPointRemapper *mF8Displace;
};

}
