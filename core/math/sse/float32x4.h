#ifndef FLOAT32X4_H_
#define FLOAT32X4_H_
/**
 * \file float32x4.h
 * \brief a header for Float32x4.c
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "core/utils/global.h"

namespace corecvs {

class ALIGN_DATA(16) Float32x4
{
public:
    __m128 data;


    /* Constructors */
    Float32x4() {}

    /**
    *  Copy constructor
    **/
    Float32x4(const Float32x4 &other) {
        this->data = other.data;
    }

    /**
    *  Constructor from intrinsic type
    **/
    explicit Float32x4(const __m128 &_data) {
        this->data = _data;
    }

    /**
    *  Constructor from integer type
    **/
    explicit Float32x4(const Int32x4 &other) {
        this->data = _mm_cvtepi32_ps(other.data);
    }

    /**
    *  Fills the vector with 4 same float values
    **/
    explicit Float32x4(const float value) {
        CORE_ASSERT_TRUE(sizeof(float) == 4, "Float should be 4 byte")
        this->data = _mm_set1_ps(value);
    }

    /**
     *  Extracts the square root from vector elements
     *  \f[
     *    \begin{array}{cccc}
     *         a_0    &    a_1    &    a_2    &    a_3    \\ \hline
     *      \sqrt a_0 & \sqrt a_1 & \sqrt a_2 & \sqrt a_3
     *    \end{array}
     *  \f]
     **/
    inline Float32x4 sqrt() const
    {
        return Float32x4(_mm_sqrt_ps(this->data));
    }

    /**
     *  truncates vector elements
     *  \f[
     *    \begin{array}{cccc}
     *        a_0   &   a_1   &   a_2   &   a_3   \\ \hline
     *      [ a_0 ] & [ a_1 ] & [ a_2 ] & [ a_3 ]
     *    \end{array}
     *  \f]
     **/
    inline Int32x4 trunc() const
    {
        return Int32x4(_mm_cvttps_epi32(this->data));
    }

    /* Arithmetics operations */
    friend Float32x4 operator +(const Float32x4 &left, const Float32x4 &right);
    friend Float32x4 operator -(const Float32x4 &left, const Float32x4 &right);

    friend Float32x4 operator +=(Float32x4 &left, const Float32x4 &right);
    friend Float32x4 operator -=(Float32x4 &left, const Float32x4 &right);

    friend Float32x4 operator *(const Float32x4 &left, const Float32x4 &right);
    friend Float32x4 operator /(const Float32x4 &left, const Float32x4 &right);

    friend Float32x4 operator *=(Float32x4 &left, const Float32x4 &right);
    friend Float32x4 operator /=(Float32x4 &left, const Float32x4 &right);

};


FORCE_INLINE Float32x4 operator +(const Float32x4 &left, const Float32x4 &right) {
    return Float32x4(_mm_add_ps(left.data, right.data));
}

FORCE_INLINE Float32x4 operator -(const Float32x4 &left, const Float32x4 &right) {
    return Float32x4(_mm_sub_ps(left.data, right.data));
}

FORCE_INLINE Float32x4 operator +=(Float32x4 &left, const Float32x4 &right) {
    left.data = _mm_add_ps(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x4 operator -=(Float32x4 &left, const Float32x4 &right) {
    left.data = _mm_sub_ps(left.data, right.data);
    return left;
}


FORCE_INLINE Float32x4 operator *(const Float32x4 &left, const Float32x4 &right) {
    return Float32x4(_mm_mul_ps(left.data, right.data));
}

FORCE_INLINE Float32x4 operator /(const Float32x4 &left, const Float32x4 &right) {
    return Float32x4(_mm_div_ps(left.data, right.data));
}

FORCE_INLINE Float32x4 operator *=(Float32x4 &left, const Float32x4 &right) {
    left.data = _mm_mul_ps(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x4 operator /=(Float32x4 &left, const Float32x4 &right) {
    left.data = _mm_div_ps(left.data, right.data);
    return left;
}

/* Some functions working with integer operations */

FORCE_INLINE Int32x4 operator /     (const Int32x4 &left, float divisor) {
    Float32x4 invDivisor(1.0f / divisor);
    return (Float32x4(left) * invDivisor).trunc();
}

FORCE_INLINE Int32x4 operator /     (const Int32x4 &left, int divisor) {
    return operator / (left, (float) divisor);
}

FORCE_INLINE Int32x4 operator /=    (Int32x4 &left, float divisor) {
    left = operator / (left, divisor);
    return left;
}

FORCE_INLINE Int32x4 operator /=    (Int32x4 &left, int divisor) {
    return operator /= (left, (float) divisor);
}


} //namespace corecvs

#endif  //FLOAT32X4_H_
