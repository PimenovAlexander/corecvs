#ifndef FIXEDPOINTDISPLACE_H_
#define FIXEDPOINTDISPLACE_H_
/**
 * \file fixedPointRemapper.h
 * \brief A header for fixedPointRemapper.cpp
 *
 *
 *
 * \date Mar 1, 2012
 * \author alexander
 * \ingroup cppcorefiles
 */

#include <stdint.h>

#include "utils/global.h"
#include "buffers/abstractBuffer.h"
#include "math/matrix/matrix33.h"
#include "buffers/fixeddisp/bilinearMapPoint.h"

namespace corecvs {

typedef AbstractBuffer<BilinearMapPoint, int32_t> FixedPointRemapperBase;

class FixedPointRemapper : public FixedPointRemapperBase /*, public DeformMap<int32_t, BilinearMapPoint>*/
{
public:

    FixedPointRemapper(int32_t h, int32_t w) : FixedPointRemapperBase (h,w) {}

    template<class Mapper>
    inline FixedPointRemapper (const Mapper &proj, int h, int w) : FixedPointRemapperBase (h,w, false)
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
