#ifndef BILINEARMAPPOINT_H_
#define BILINEARMAPPOINT_H_
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

namespace corecvs {

/**
 *   This struct holds the data for fast bilinear transform computation
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

} // namespace corecvs

#endif  /* #ifndef BILINEARMAPPOINT_H_ */
