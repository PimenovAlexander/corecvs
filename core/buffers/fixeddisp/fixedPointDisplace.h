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

#include "global.h"
#include "abstractContiniousBuffer.h"
#include "matrix33.h"

namespace corecvs {

/**
 *   This function holds the data for fast bilinear transform computation
 **/
struct BilinearMapPoint
{
    uint16_t x;
    uint16_t y;

    uint8_t k1;
    uint8_t k2;
    uint8_t k3;
    uint8_t k4;

   BilinearMapPoint() {}

   BilinearMapPoint(double newx, double newy)
   {
        x = (uint16_t)floor(newx);
        y = (uint16_t)floor(newy);

        double dx = newx - x;
        double dy = newy - y;

        /* Common subexpression could be extracted here */
        k1 = (uint8_t)((1.0 - dx) * (1.0 - dy) * 255.0);
        k2 = (uint8_t)(       dx  * (1.0 - dy) * 255.0);
        k3 = (uint8_t)((1.0 - dx) *        dy  * 255.0);
        k4 = (uint8_t)(       dx  *        dy  * 255.0);
   }
};

typedef AbstractContiniousBuffer<BilinearMapPoint, int32_t> FixedPointDisplaceBase;

class FixedPointDisplace : public FixedPointDisplaceBase /*, public DeformMap<int32_t, BilinearMapPoint>*/
{
public:

    FixedPointDisplace(int32_t h, int32_t w) : FixedPointDisplaceBase (h,w) {}

    inline FixedPointDisplace (const Matrix33 &proj, int h, int w) : FixedPointDisplaceBase (h,w, false)
    {
        int i,j;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j++)
            {
                Vector2dd result   = proj * Vector2dd(j,i);
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
