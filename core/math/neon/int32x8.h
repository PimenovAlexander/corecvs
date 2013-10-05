#ifndef INT32X8_H_
#define INT32X8_H_
/**
 * \file int32x8.h
 * \brief a header for Int32x8.c
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */
#include "global.h"
#include "vector2d.h"
namespace corecvs {

class Int32x8 : public Vector2d<Int32x4>
{
public:
    explicit Int32x8(const Int32x4 &lowAddr, const Int32x4 &highAddr): Vector2d<Int32x4>(lowAddr, highAddr){};

    explicit Int32x8(int32_t value): Vector2d<Int32x4>(Int32x4(value)){};

    explicit Int32x8(const int32x4_t &_data0, const int32x4_t &_data1)
    {
        element[0].data = _data0;
        element[1].data = _data1;
    }

    /**
     * TODO: Bring this to FixedVector
     * */
    friend Int32x8 operator >>   (const Int32x8 &left, uint32_t count);
    friend Int32x8 operator >>=  (Int32x8 &left, uint32_t count);

    inline Int32x8 &operator /=(int c)
    {
        this->element[0] /= c;
        this->element[1] /= c;
        return *this;
    }

};

FORCE_INLINE inline Int32x8 operator >>  (const Int32x8 &left, uint32_t count) {
    return Int32x8 ( left[0] >> count, left[1] >> count);

}

FORCE_INLINE inline Int32x8 operator >>=(Int32x8 &left, uint32_t count) {
    left[0] >>= count;
    left[1] >>= count;
    return left;
}



} //namespace corecvs
#endif  //INT32X8_H_

