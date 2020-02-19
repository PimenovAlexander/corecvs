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

#include "utils/global.h"
namespace corecvs {

class ALIGN_DATA(16) Float32x4
{
public:
    float32x4_t data;


    /* Constructors */
    Float32x4(){}

    /**
    *  Copy constructor
    **/
    Float32x4(const Float32x4 &other) {
        this->data = other.data;
    }

    Float32x4(const float32x4_t &_data) {
        this->data = _data;
    }

    Float32x4(const Int32x4 &other) {
        this->data = vcvtq_f32_s32(other.data);
    }

    explicit Float32x4(const float value) {
        // CORE_ASSERT_TRUE(sizeof(float) == 4, "Float should be 4 byte")
        this->data = vdupq_n_f32(value);
    }

    inline Float32x4 sqrt() const
    {
        return vrecpeq_f32(vrsqrteq_f32(this->data));
    }

    inline Int32x4 trunc() const
    {
        return vcvtq_s32_f32(this->data);

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
    return vaddq_f32(left.data, right.data);
}

FORCE_INLINE Float32x4 operator -(const Float32x4 &left, const Float32x4 &right) {
    return vsubq_f32(left.data, right.data);
}

FORCE_INLINE Float32x4 operator +=(Float32x4 &left, const Float32x4 &right) {
    left.data = vaddq_f32(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x4 operator -=(Float32x4 &left, const Float32x4 &right) {
    left.data = vsubq_f32(left.data, right.data);
    return left;
}


FORCE_INLINE Float32x4 operator *(const Float32x4 &left, const Float32x4 &right) {
    return vmulq_f32(left.data, right.data);
}

FORCE_INLINE Float32x4 operator /(const Float32x4 &left, const Float32x4 &right) {
    return vmulq_f32(left.data, vrecpeq_f32(right.data));
}

FORCE_INLINE Float32x4 operator *=(Float32x4 &left, const Float32x4 &right) {
    left.data = vmulq_f32(left.data, right.data);
    return left;
}

FORCE_INLINE Float32x4 operator /=(Float32x4 &left, const Float32x4 &right) {
    left.data = vmulq_f32(left.data, vrecpeq_f32(right.data));
    return left;
}

/* Some functions working with integer operations */
FORCE_INLINE Int32x4 operator /     (const Int32x4 &left, float divisor){
    Float32x4 invDivisor(1.0f / divisor);
    return (Float32x4(left) * invDivisor).trunc();
}

FORCE_INLINE Int32x4 operator /     (const Int32x4 &left, int divisor) {
    return operator / (left, (float) divisor);
}

FORCE_INLINE Int32x4 operator /=    (Int32x4 &left, float divisor){
    left = operator / (left, divisor);
    return left;
}

FORCE_INLINE Int32x4 operator /=    (Int32x4 &left, int divisor){
    return operator /= (left, (float) divisor);
}




} //namespace corecvs
#endif  //FLOAT32X4_H_

