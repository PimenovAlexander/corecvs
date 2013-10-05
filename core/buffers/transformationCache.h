#pragma once
#include "displacementBuffer.h"
#include "generated/interpolationType.h"

namespace corecvs {

/**
 *  This class is caching the transformation of the buffer.
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
    G12Buffer *doDeformation(InterpolationType::InterpolationType type, G12Buffer *buffer);
    ~TransformationCache();

private:
    DirectRemapper *mRemap;
    DisplacementBuffer *mDisplace;
    FixedPointDisplace *mF8Displace;
};

}
