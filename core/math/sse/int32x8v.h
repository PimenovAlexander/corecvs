#ifndef INT32X8V_H_
#define INT32X8V_H_
/**
 * \file int32x8v.h
 * \brief a header for Int32x8v.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */
#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/math/sse/int32x4.h"

namespace corecvs {

class Int32x8v : public Vector2d<Int32x4>
{
public:
    static const int SIZE = 8;

    explicit Int32x8v(const Int32x4 &lowAddr, const Int32x4 &highAddr) : Vector2d<Int32x4>(lowAddr, highAddr) {}

    explicit Int32x8v(int32_t value) : Vector2d<Int32x4>(Int32x4(value)) {}

    explicit Int32x8v(const __m128i &_data0, const __m128i &_data1)
    {
        element[0].data = _data0;
        element[1].data = _data1;
    }

    /**
     * TODO: Bring this to FixedVector
     * */
    friend Int32x8v operator >>  (const Int32x8v &left, uint32_t count);
    friend Int32x8v operator >>= (      Int32x8v &left, uint32_t count);

    inline Int32x8v &operator /=(int c)
    {
        this->element[0] /= c;
        this->element[1] /= c;
        return *this;
    }

    /* Formated hex helper */
    void hexDump (ostream &out);
};

FORCE_INLINE Int32x8v operator >> (const Int32x8v &left, uint32_t count) {
    return Int32x8v (left[0] >> count, left[1] >> count);
}

FORCE_INLINE Int32x8v operator >>= (Int32x8v &left, uint32_t count) {
    left[0] >>= count;
    left[1] >>= count;
    return left;
}


} //namespace corecvs

#endif  //INT32X8V_H_
