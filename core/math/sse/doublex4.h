#ifndef DOUBLEX4_H
#define DOUBLEX4_H

/**
 * \file doublex4.h
 * \brief a header for Doublex4.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "core/utils/global.h"
#include <immintrin.h>


namespace corecvs {

class ALIGN_DATA(32) Doublex4
{
public:
    __m256d data;

    static const int SIZE = 4;

    Doublex4() : data(_mm256_setzero_pd())
    {}

    explicit Doublex4(const double value) {
        this->data = _mm256_set1_pd(value);
    }

    /**
    *  Constructor from intrinsic type
    **/
    explicit Doublex4(const __m256d &_data) {
        this->data = _data;
    }

    explicit Doublex4(double value1, double value2, double value3, double value4) {
        this->data = _mm256_set_pd(value1, value2, value3, value4);
    }

    explicit Doublex4(const double * const data_ptr)
    {
        this->data = _mm256_loadu_pd(data_ptr);
    }

    inline static Doublex4 Zero()
    {
        return Doublex4(_mm256_setzero_pd());
    }

    inline static Doublex4 Broadcast(const double *data_ptr)
    {
        return Doublex4(_mm256_broadcast_sd(data_ptr));
    }

    void load(double * const data)
    {
        this->data = _mm256_loadu_pd(data);
    }

    void save(double * const data) const
    {
        _mm256_storeu_pd(data, this->data);
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(double * const data) const
    {
        _mm256_store_pd(data, this->data);
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(double * const data) const
    {
        _mm256_stream_pd(data, this->data);
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
    inline Doublex4 sqrt() const
    {
        return Doublex4(_mm256_sqrt_pd(this->data));
    }

    /* Something stupid happens here. */
template<int idx>
    double getDouble() const
    {
        double storage[SIZE];
        save(storage);
        return storage[idx];
    }

    inline double operator[] (uint32_t idx) const
    {
        switch (idx) {
            case 0: return getDouble<0>();
            case 1: return getDouble<1>();
            case 2: return getDouble<2>();
            case 3: return getDouble<3>();
        }
        return 0;
    }




    /* Arithmetics operations */
    Doublex4 operator -()
    {
        return Doublex4::Zero() - *this;
    }

    friend Doublex4 operator +(const Doublex4 &left, const Doublex4 &right);
    friend Doublex4 operator -(const Doublex4 &left, const Doublex4 &right);

    friend Doublex4 operator +=(Doublex4 &left, const Doublex4 &right);
    friend Doublex4 operator -=(Doublex4 &left, const Doublex4 &right);

    friend Doublex4 operator *(const Doublex4 &left, const Doublex4 &right);
    friend Doublex4 operator /(const Doublex4 &left, const Doublex4 &right);

    friend Doublex4 operator *=(Doublex4 &left, const Doublex4 &right);
    friend Doublex4 operator /=(Doublex4 &left, const Doublex4 &right);

    /*Print to stream helper */
    friend std::ostream & operator << (std::ostream &out, const Doublex4 &vector);

};

FORCE_INLINE Doublex4 operator +(const Doublex4 &left, const Doublex4 &right) {
    return Doublex4(_mm256_add_pd(left.data, right.data));
}

FORCE_INLINE Doublex4 operator -(const Doublex4 &left, const Doublex4 &right) {
    return Doublex4(_mm256_sub_pd(left.data, right.data));
}

FORCE_INLINE Doublex4 operator +=(Doublex4 &left, const Doublex4 &right) {
    left.data = _mm256_add_pd(left.data, right.data);
    return left;
}

FORCE_INLINE Doublex4 operator -=(Doublex4 &left, const Doublex4 &right) {
    left.data = _mm256_sub_pd(left.data, right.data);
    return left;
}


FORCE_INLINE Doublex4 operator *(const Doublex4 &left, const Doublex4 &right) {
    return Doublex4(_mm256_mul_pd(left.data, right.data));
}

FORCE_INLINE Doublex4 operator /(const Doublex4 &left, const Doublex4 &right) {
    return Doublex4(_mm256_div_pd(left.data, right.data));
}

FORCE_INLINE Doublex4 operator *=(Doublex4 &left, const Doublex4 &right) {
    left.data = _mm256_mul_pd(left.data, right.data);
    return left;
}

FORCE_INLINE Doublex4 operator /=(Doublex4 &left, const Doublex4 &right) {
    left.data = _mm256_div_pd(left.data, right.data);
    return left;
}

FORCE_INLINE Doublex4 multiplyAdd(const Doublex4 &mul1, const Doublex4 &mul2, const Doublex4 &add1) {
#ifdef WITH_FMA
    return Doublex4(_mm256_fmadd_pd(mul1.data, mul2.data, add1.data));
#else
    return mul1 * mul2 + add1;
#endif
}



} //namespace corecvs

#endif // DOUBLEX4_H
