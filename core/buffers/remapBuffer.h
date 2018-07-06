#ifndef REMAPBUFFER_H
#define REMAPBUFFER_H

#include <stdint.h>

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/buffers/abstractContiniousBuffer.h"
#include "core/buffers/g12Buffer.h"
#include "core/math/vector/vector2d.h"
#include "core/math/matrix/matrix33.h"
#include "core/tbbwrapper/tbbWrapper.h"
#include "core/alignment/radialCorrection.h"
#include "core/alignment/distortionCorrectTransform.h"
#include "core/alignment/lensDistortionModelParameters.h"
#include "../math/levenmarq.h"

namespace corecvs {

typedef AbstractContiniousBuffer<Vector2dd, int32_t> RemapBufferBase;

/**
 *   This buffer stores a buffer deformation - a shift of the buffer pixels.
 *
 *   This buffer is usually used to cache some sort of transformation that is
 *   too slow to compute.
 *
 *   \attention usually you should use this to store inverse transformation.
 *
 */

class RemapBuffer : public RemapBufferBase, public DeformMap<int32_t, double>
{
public:

    RemapBuffer(int32_t h = 0, int32_t w = 0) : RemapBufferBase (h, w) {}
    RemapBuffer(const Vector2d32 &size)       : RemapBufferBase (size) {}


    RemapBuffer(const RemapBuffer &that)      : RemapBufferBase (that) {}
    RemapBuffer(RemapBuffer *that)            : RemapBufferBase (that) {}

    RemapBuffer(RemapBuffer *src, int32_t x1, int32_t y1, int32_t x2, int32_t y2) :
        RemapBufferBase(src, x1, y1, x2, y2) {}

    RemapBuffer(int32_t h, int32_t w, Vector2dd *data) :
        RemapBufferBase(h, w, data) {}

    RemapBuffer(int32_t h, int32_t w, Vector2dd data) :
        RemapBufferBase(h, w, data) {}


    inline RemapBuffer (Matrix33 *proj, int h, int w) : RemapBufferBase (h, w, false)
    {
        int i,j;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                Vector2dd result = ((*proj) * Vector2dd(j,i));
                this->element(i,j) = result;
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

    static RemapBuffer *CacheInverse(
            RadialCorrection *inverseMap,
            int h, int w,
            double x1, double y1,
            double x2, double y2,
            double step = 0.5,
            bool useLM = false
    );

    static RemapBuffer *TestWiggle(
            int h, int w,
            double power = 5,
            double step = 20
    );

    inline RemapBuffer (DistortionCorrectTransform *transform, int h, int w) : RemapBufferBase (h, w, false)
    {
        for (int i = 0; i < h; i ++)
        {
            for (int j = 0; j < w; j ++)
            {
                Vector2dd mapped = transform->map(i, j);
                Vector2dd result = mapped;
                this->element(i, j) = result;
            }
        }
    }

    inline Vector2dd map(const uint32_t y, const uint32_t x) const
    {
        Vector2d32 pointi(x, y);
        return this->element(pointi);
    }

    /*Bilinear approach should be used*/
    inline Vector2dd map(Vector2dd pointd) const
    {
        Vector2d32 pointi(pointd.x(), pointd.y());
        return this->element(pointi);
    }

};


} // namespace

#endif // REMAPBUFFER_H
