/**
 * \file displacementBuffer.h
 * \brief This file hold the class DisplacementBuffer
 *
 * \ingroup cppcorefiles
 * \date Mar 21, 2010
 * \author alexander
 */

#ifndef CDISPLACEMENTBUFFER_H_
#define CDISPLACEMENTBUFFER_H_

#include <stdint.h>

#include "global.h"

#include "mathUtils.h"
#include "abstractContiniousBuffer.h"
#include "g12Buffer.h"
#include "vector2d.h"
#include "matrix33.h"
#include "tbbWrapper.h"
#include "radialCorrection.h"
#include "distortionCorrectTransform.h"
#include "../math/levenmarq.h"

namespace corecvs {

typedef AbstractContiniousBuffer<Vector2dd, int32_t> DisplacementBufferBase;

/**
 *   This buffer stores a buffer deformation - a shift of the buffer pixels.
 *
 *   This buffer is usually used to cache some sort of transformation that is
 *   to slow to compute.
 *
 *   \attention usually you should use this to store inverse transformation.
 *
 */

class DisplacementBuffer : public DisplacementBufferBase, public DeformMap<int32_t, double>
{
public:

    DisplacementBuffer(int32_t h, int32_t w)     : DisplacementBufferBase (h, w) {}
    DisplacementBuffer(DisplacementBuffer &that) : DisplacementBufferBase (that) {}
    DisplacementBuffer(DisplacementBuffer *that) : DisplacementBufferBase (that) {}

    DisplacementBuffer(DisplacementBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        DisplacementBufferBase(src, x1, y1, x2, y2) {}

    DisplacementBuffer(int32_t h, int32_t w, Vector2dd *data) :
        DisplacementBufferBase(h, w, data) {}

    DisplacementBuffer(int32_t h, int32_t w, Vector2dd data) :
        DisplacementBufferBase(h, w, data) {}


    inline DisplacementBuffer (Matrix33 *proj, int h, int w) : DisplacementBufferBase (h, w, false)
    {
        int i,j;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                Vector2dd result = ((*proj) * Vector2dd(j,i)) - Vector2dd(j,i);
                this->element(i,j) = result;
            }
        }
    }

    inline DisplacementBuffer (RadialCorrection *inverseMap, int h, int w, bool isInverse) : DisplacementBufferBase (h, w, false)
    {
        int koef = isInverse ? 1 : -1;
        for (int i = 0; i < h; i ++)
        {
            for (int j = 0; j < w; j ++)
            {
                Vector2dd mapped = inverseMap->map(i, j);
                Vector2dd result = (Vector2dd(j, i) - mapped) * koef;
                this->element(i, j) = result;
            }
        }
    }

    static DisplacementBuffer *CacheInverse(
            RadialCorrection *inverseMap,
            int h, int w,
            double x1, double y1,
            double x2, double y2,
            double step = 0.5,
            bool useLM = false
    );

    inline DisplacementBuffer (DistortionCorrectTransform *transform, int h, int w) : DisplacementBufferBase (h, w, false)
    {
        for (int i = 0; i < h; i ++)
        {
            for (int j = 0; j < w; j ++)
            {
                Vector2dd mapped = transform->map(i, j);
                Vector2dd result = - Vector2dd(j, i) + mapped;
                this->element(i, j) = result;
            }
        }
    }

    inline Vector2dd map(const uint32_t y, const uint32_t x) const
    {
        Vector2dd  pointd(x, y);
        Vector2d32 pointi(x, y);
        return element(pointi) + pointd;
    }

};


/**
 * TODO: rewrite this based on AbstractBuffer
 * This class caches the image transformation
 *
 * for each output pixel we store its position at the input one.
 *
 * This class design under development
 *
 **/
class DirectRemapper
{
public:

    int mInputH;
    int mInputW;
    int mOutputH;
    int mOutputW;

    Vector2d32 *mOffsets;

    inline DirectRemapper (Matrix33 *proj, Vector2d<int> inputSize, Vector2d<int> outputSize)
    {
        _init (proj, inputSize.y(), inputSize.x(), outputSize.y(), outputSize.x());
    }

    inline DirectRemapper (Matrix33 *proj, int inputH, int inputW, int outputH, int outputW)
    {
        _init (proj, inputH, inputW, outputH, outputW);
    }

    inline void _init (Matrix33 *proj, int inputH, int inputW, int outputH, int outputW)
    {
        mInputH  = inputH;
        mInputW  = inputW;
        mOutputH = outputH;
        mOutputW = outputW;
        DOTRACE(("DirectRemapper: h= %d, w= %d\n", mOutputH, mOutputW));

        mOffsets = new Vector2d32[mOutputH * mOutputW];
        int i,j;
        for (i = 0; i < mOutputH; i++)
        {
            for (j = 0; j < mOutputW; j++)
            {
                Vector3dd data = (*proj) * Vector3dd(j,i,1);
                data = data / data.z();

                int oldX = fround(data.x());
                int oldY = fround(data.y());
                if (oldY < 0 || oldY >= mInputH || oldX < 0 || oldX >= mInputW)
                    mOffsets[i * mOutputW + j] = Vector2d32(0, 0);
                else
                    mOffsets[i * mOutputW + j] = Vector2d32(oldX, oldY);
            }
        }

        mOutputW = outputW;
    }

    /* Class for parallel processing of the remap */
    class ParallelRemap
    {
        G12Buffer *result;
        G12Buffer *input;
        DirectRemapper *remapper;

    public:
        void operator()( const BlockedRange<int>& r ) const
        {
            for (int i = r.begin(); i < r.end(); i++)
            {
                uint16_t *offsetResult = &result->element(i, 0);
                Vector2d32 *offsetMap  = &remapper->mOffsets[i * remapper->mOutputW];
                for (int j = 0; j < remapper->mOutputW; j++)
                {
                    Vector2d32 oldCoord = *offsetMap++;
                    *offsetResult++ = input->element(oldCoord);
                }
            }
        }
        ParallelRemap(G12Buffer *_result, G12Buffer *_input, DirectRemapper *_remapper) :
        result(_result), input(_input), remapper(_remapper)
        {}
    };

    G12Buffer *remap (G12Buffer *input)
    {
        ASSERT_TRUE(input->h == mInputH && input->w == mInputW, "Wrong size...");
        G12Buffer *result = new G12Buffer(mOutputH, mOutputW);
        parallelable_for(0, mOutputH, ParallelRemap(result, input, this));
        return result;
    }

    ~DirectRemapper()
    {
        deletearr_safe(mOffsets);
    }

};


} //namespace corecvs

#endif /* CDISPLACEMENTBUFFER_H_ */
