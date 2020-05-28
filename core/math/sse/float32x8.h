#ifndef FLOAT32X8_H_
#define FLOAT32X8_H_
/**
 * \file float32x8.h
 * \brief a header for Float32x8.c
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "utils/global.h"
#include "math/avx/int32x8.h"

namespace corecvs {

class ALIGN_DATA(32) Float32x8
{
public:
    __m256 data;

    static const int SIZE = 8;

    /* Constructors */
    Float32x8() {}

    /**
    *  Copy constructor
    **/
    Float32x8(const Float32x8 &other) {
        this->data = other.data;
    }

    /**
    *  Constructor from intrinsic type
    **/
    explicit Float32x8(const __m256 &_data) {
        this->data = _data;
    }


    explicit Float32x8(const float * const data_ptr)
    {
        this->data = _mm256_loadu_ps(data_ptr);
    }

    /**
    *  Constructor from integer type
    **/
    explicit Float32x8(const Int32x8 &other) {
        this->data = _mm256_cvtepi32_ps(other.data);
    }

    inline static Float32x8 Zero()
    {
        return Float32x8(_mm256_setzero_ps());
    }

    inline static Float32x8 Broadcast(const float *data_ptr)
    {
        return Float32x8(_mm256_broadcast_ss(data_ptr));
    }

    /**
    *  Fills the vector with 4 same float values
    **/
    explicit Float32x8(const float value) {
        CORE_ASSERT_TRUE(sizeof(float) == 4, "Float should be 4 byte")
        this->data = _mm256_set1_ps(value);
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
    inline Float32x8 sqrt() const
    {
        return Float32x8(_mm256_sqrt_ps(this->data));
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
    inline Int32x8 trunc() const
    {
        return Int32x8(_mm256_cvttps_epi32(this->data));
    }


    void load(float * const data)
    {
        this->data = _mm256_loadu_ps(data);
    }

    void save(float * const data) const
    {
        _mm256_storeu_ps(data, this->data);
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(float * const data) const
    {
        _mm256_store_ps(data, this->data);
    }

    /* Arithmetics operations */
    friend Float32x8 operator +(const Float32x8 &left, const Float32x8 &right);
    friend Float32x8 operator -(const Float32x8 &left, const Float32x8 &right);

    friend Float32x8 operator +=(Float32x8 &left, const Float32x8 &right);
    friend Float32x8 operator -=(Float32x8 &left, const Float32x8 &right);

    friend Float32x8 operator *(const Float32x8 &left, const Float32x8 &right);
    friend Float32x8 operator /(const Float32x8 &left, const Float32x8 &right);

    friend Float32x8 operator *=(Float32x8 &left, const Float32x8 &right);
    friend Float32x8 operator /=(Float32x8 &left, const Float32x8 &right);

};


FORCE_INLINE Float32x8 operator +(const Float32x8 &left, const Float32x8 &right) {
    return Float32x8(_mm256_add_ps(left.data, right.data));
}

FORCE_INLINE Float32x8 operator -(const Float32x8 &left, const Float32x8 &right) {
    return Float32x8(_mm256_sub_ps(left.data, right.data));
}

FORCE_INLINE Float32x8 operator +=(Float32x8 &left, const Float32x8 &right) {
    left.data = _mm256_add_ps(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x8 operator -=(Float32x8 &left, const Float32x8 &right) {
    left.data = _mm256_sub_ps(left.data, right.data);
    return left;
}


FORCE_INLINE Float32x8 operator *(const Float32x8 &left, const Float32x8 &right) {
    return Float32x8(_mm256_mul_ps(left.data, right.data));
}

FORCE_INLINE Float32x8 operator /(const Float32x8 &left, const Float32x8 &right) {
    return Float32x8(_mm256_div_ps(left.data, right.data));
}

FORCE_INLINE Float32x8 operator *=(Float32x8 &left, const Float32x8 &right) {
    left.data = _mm256_mul_ps(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x8 operator /=(Float32x8 &left, const Float32x8 &right) {
    left.data = _mm256_div_ps(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x8 multiplyAdd(const Float32x8 &mul1, const Float32x8 &mul2, const Float32x8 &add1) {
#ifdef WITH_FMA
    return Float32x8(_mm256_fmadd_ps(mul1.data, mul2.data, add1.data));
#else
    return mul1 * mul2 + add1;
#endif
}

/* Some functions working with integer operations */

/*
FORCE_INLINE Int32x4 operator /     (const Int32x4 &left, float divisor) {
    Float32x8 invDivisor(1.0f / divisor);
    return (Float32x8(left) * invDivisor).trunc();
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
*/

} //namespace corecvs

#endif  //FLOAT32X8_H_
