#ifndef DISPLACEMENT_BUFFER_H_
#define DISPLACEMENT_BUFFER_H_
/**
 * \file displacementBuffer.h
 * \brief This file hold the class DisplacementBuffer
 *
 * \ingroup cppcorefiles
 * \date Mar 21, 2010
 * \author alexander
 */


#include <stdint.h>

#include "utils/global.h"

#include "math/mathUtils.h"
#include "buffers/abstractContiniousBuffer.h"
#include "buffers/g12Buffer.h"
#include "math/vector/vector2d.h"
#include "math/matrix/matrix33.h"
#include "tbbwrapper/tbbWrapper.h"
#include "alignment/radialCorrection.h"
#include "alignment/distortionCorrectTransform.h"
#include "alignment/lensDistortionModelParameters.h"
#include "../math/levenmarq.h"

namespace corecvs {

typedef AbstractContiniousBuffer<Vector2dd, int32_t> DisplacementBufferBase;

/**
 *   This buffer stores a buffer deformation - a shift of the buffer pixels.
 *
 *   This buffer is usually used to cache some sort of transformation that is
 *   too slow to compute.
 *
 *   \attention usually you should use this to store inverse transformation.
 *
 */

class DisplacementBuffer : public DisplacementBufferBase, public DeformMap<int32_t, double>
{
public:

    DisplacementBuffer(int32_t h = 0, int32_t w = 0)     : DisplacementBufferBase (h, w) {}
    DisplacementBuffer(const DisplacementBuffer &that) : DisplacementBufferBase (that) {}
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

    inline DisplacementBuffer (RadialCorrection *inverseMap, int h, int w, bool isInverse = false) : DisplacementBufferBase (h, w, false)
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

    inline DisplacementBuffer (LensDistortionModelParameters &ldmp, int h, int w) : DisplacementBufferBase (h, w, false)
    {
        int koef = -1;
        for (int i = 0; i < h; i ++)
        {
            for (int j = 0; j < w; j ++)
            {
                Vector2dd mapped = ldmp.map(Vector2dd(j, i));
                Vector2dd result = (Vector2dd(j, i) - mapped) * koef;
                this->element(i, j) = result;
            }
        }
    }

    /**
     * This fills current displacement buffer with the inverse of the RadialCorrection
     *   This is a two stage algorithm that first iterates over the target area with the given step
     *
     * \param inv
     *
     * Rectangle that should be inverted. Target is [0,0] x [w,h] - to change target area (which will become source after inversion) please
     * use TableInverseCache
     *
     * \param x1
     * \param y1
     * \param x2
     * \param y2
     *
     * \param useLM Should we use second stage that makes inversion subpixel
     *
     *
     **/
    void fillWithInvertedRadialBase(
            RadialCorrection *inverseMap,
            double x1, double y1,
            double x2, double y2,
            double step = 0.5,
            bool useLM = false
    );

    static DisplacementBuffer *CacheInverse(
            RadialCorrection *inverseMap,
            int h, int w,
            double x1, double y1,
            double x2, double y2,
            double step = 0.5,
            bool useLM = false
    );

    static DisplacementBuffer *TestWiggle(
            int h, int w,
            double power = 5,
            double step = 20
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
        return this->element(pointi) + pointd;
    }

    /*Bilinear approach should be used*/
    inline Vector2dd map(Vector2dd pointd) const
    {
        Vector2d32 pointi(pointd.x(), pointd.y());
        return this->element(pointi) + pointd;
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
    template<class BufferToTransform>
    class ParallelRemap
    {
        BufferToTransform *result;
        BufferToTransform *input;
        DirectRemapper *remapper;

        typedef typename BufferToTransform::InternalElementType ResultElement;
    public:
        void operator()( const BlockedRange<int>& r ) const
        {
            for (int i = r.begin(); i < r.end(); i++)
            {
                ResultElement *offsetResult = &result->element(i, 0);
                Vector2d32    *offsetMap  = &remapper->mOffsets[i * remapper->mOutputW];

                for (int j = 0; j < remapper->mOutputW; j++)
                {
                    *offsetResult = input->element(*offsetMap);

                    offsetMap++;
                    offsetResult++;
                }
            }
        }

        ParallelRemap(
                BufferToTransform *_result,
                BufferToTransform *_input,
                DirectRemapper *_remapper) :
            result(_result),
            input(_input),
            remapper(_remapper)
        {}
    };

    template<class BufferToTransform>
    BufferToTransform *remap (BufferToTransform *input)
    {
        CORE_ASSERT_TRUE(input->h == mInputH && input->w == mInputW, "Wrong size...");
        BufferToTransform *result = new BufferToTransform(mOutputH, mOutputW);
        parallelable_for(0, mOutputH, ParallelRemap<BufferToTransform>(result, input, this));
        return result;
    }

    ~DirectRemapper()
    {
        deletearr_safe(mOffsets);
    }

};


class TableInverseCache : public DisplacementBuffer
{
public:
    Vector2dd inputShift = Vector2dd(0.0, 0.0);

    TableInverseCache(int32_t h = 0, int32_t w = 0) : DisplacementBuffer (h, w) {}

    static TableInverseCache* CacheInverse(
            int tx1, int ty1,
            int tx2, int ty2,
            RadialCorrection *inverseMap,
            double x1, double y1,
            double x2, double y2,
            double step = 0.5,
            bool useLM = false
    );


    bool isValidCoord (const int32_t y, const int32_t x) const
    {
        //SYNC_PRINT(("C"));
        return DisplacementBuffer::isValidCoord(y - inputShift.y(), x - inputShift.x());
    }

    bool isValidCoord (const Vector2d32 &pointd) const
    {
        return isValidCoord(pointd.y(), pointd.x());
    }

    bool isValidCoordBl (const double y, const double x) const
    {
        return DisplacementBuffer::isValidCoordBl(y - inputShift.y(), x - inputShift.x());
    }

    bool isValidCoordBl (const Vector2dd &pointd) const
    {
        return isValidCoordBl(pointd.y(), pointd.x());
    }


    inline Vector2dd map(const int32_t y, const int32_t x) const
    {
        Vector2dd  pointd(x, y);
        Vector2d32 pointi(x - inputShift.x(), y - inputShift.y());
        if (DisplacementBuffer::isValidCoord(pointi.y(), pointi.x()))
            return this->element(pointi) + pointd - inputShift;
        else
            return Vector2dd(0,0);
    }

    /*Bilinear approach should be used*/
    inline Vector2dd map(const Vector2dd &pointd) const
    {
        Vector2d32 pointi(pointd.x() - inputShift.x(), pointd.y() - inputShift.y());
        if (DisplacementBuffer::isValidCoordBl(pointi.y(), pointi.x()))
            return this->element(pointi) + pointd - inputShift;
        else
            return Vector2dd(0,0);
    }



};

} //namespace corecvs

#endif /* DISPLACEMENT_BUFFER_H_ */
