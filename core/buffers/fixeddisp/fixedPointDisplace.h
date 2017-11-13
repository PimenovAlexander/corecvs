#ifndef FIXEDPOINTDISPLACE_H_
#define FIXEDPOINTDISPLACE_H_
/**
 * \file fixedPointDisplace.h
 * \brief A header for fixedPointDisplace.c
 *
 *
 *
 * \date Mar 1, 2012
 * \author alexander
 * \ingroup cppcorefiles
 */

#include <stdint.h>

#include "core/utils/global.h"
#include "core/buffers/abstractBuffer.h"
#include "core/math/matrix/matrix33.h"
#include "core/buffers/fixeddisp/bilinearMapPoint.h"

namespace corecvs {

typedef AbstractBuffer<BilinearMapPoint, int32_t> FixedPointDisplaceBase;

class FixedPointDisplace : public FixedPointDisplaceBase /*, public DeformMap<int32_t, BilinearMapPoint>*/
{
public:

    FixedPointDisplace(int32_t h, int32_t w) : FixedPointDisplaceBase (h,w) {}

    template<class Mapper>
    inline FixedPointDisplace (const Mapper &proj, int h, int w) : FixedPointDisplaceBase (h,w, false)
    {
        int i,j;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                Vector2dd result   = proj.map(Vector2dd(j,i));
                this->element(i,j) = BilinearMapPoint(result.x(), result.y());
            }
        }
    }

    inline BilinearMapPoint map(const uint32_t y, const uint32_t x) const
    {
        return element(y, x);
    }

};

} // namespace corecvs

#endif  /* #ifndef FIXEDPOINTDISPLACE_H_ */ 
