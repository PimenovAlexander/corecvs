#ifndef INT32X16V_H_
#define INT32X16V_H_
/**
 * \file int32x16v.h
 * \brief a header for Int32x16v.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */
#include "core/utils/global.h"

#include "core/math/vector/vector2d.h"
#include "core/math/avx/int32x8.h"

namespace corecvs {

class Int32x16v : public Vector2d<Int32x8>
{
public:
    static const int SIZE = 16;

    explicit Int32x16v(const Int32x8 &lowAddr, const Int32x8 &highAddr) : Vector2d<Int32x8>(lowAddr, highAddr) {}

    explicit Int32x16v(int32_t value) : Vector2d<Int32x8>(Int32x8(value)) {}

    explicit Int32x16v(const __m256i &_data0, const __m256i &_data1)
    {
        element[0].data = _data0;
        element[1].data = _data1;
    }

    /**
     * TODO: Bring this to FixedVector
     * */
    friend Int32x16v operator >>  (const Int32x16v &left, uint32_t count);
    friend Int32x16v operator >>= (      Int32x16v &left, uint32_t count);

    inline Int32x16v &operator /=(int c)
    {
        this->element[0] /= c;
        this->element[1] /= c;
        return *this;
    }

    /* Formated hex helper */
    void hexDump (std::ostream &out);
};

FORCE_INLINE Int32x16v operator >> (const Int32x16v &left, uint32_t count) {
    return Int32x16v (left[0] >> count, left[1] >> count);
}

FORCE_INLINE Int32x16v operator >>= (Int32x16v &left, uint32_t count) {
    left[0] >>= count;
    left[1] >>= count;
    return left;
}


} //namespace corecvs

#endif  //INT32X16V_H_
