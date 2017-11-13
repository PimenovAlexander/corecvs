#ifndef DOUBLEX2_H
#define DOUBLEX2_H

/**
 * \file doublex4.h
 * \brief a header for Doublex2.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "core/utils/global.h"

#ifdef WITH_FMA
#   include <immintrin.h>
#endif

namespace corecvs {

class ALIGN_DATA(16) Doublex2
{
public:
    __m128d data;

    static const int SIZE = 2;

    Doublex2() {}

    explicit Doublex2(const double value) {
        this->data = _mm_set1_pd(value);
    }

    /**
    *  Constructor from intrinsic type
    **/
    explicit Doublex2(const __m128d &_data) {
        this->data = _data;
    }

    explicit Doublex2(double value1, double value2 ) {
        this->data = _mm_set_pd(value1, value2);
    }

    explicit Doublex2(const double * const data_ptr)
    {
        this->data = _mm_loadu_pd(data_ptr);
    }

    static inline Doublex2 Zero()
    {
        return Doublex2(_mm_setzero_pd());
    }

    static inline Doublex2 Broadcast(const double *data_ptr)
    {
        return Doublex2(_mm_loaddup_pd(data_ptr));
    }

    void save(double * const data) const
    {
        _mm_storeu_pd(data, this->data);
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(double * const data) const
    {
        _mm_store_pd(data, this->data);
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(double * const data) const
    {
        _mm_stream_pd(data, this->data);
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
    inline Doublex2 sqrt() const
    {
        return Doublex2(_mm_sqrt_pd(this->data));
    }

template<int idx>
    double getDouble() const
    {
        double storage[2];
        save(storage);
        return storage[idx];
    }

    inline double operator[] (uint32_t idx) const
    {
        switch (idx) {
            case 0: return getDouble<0>();
            case 1: return getDouble<1>();
        }
        return 0;
    }

    /* Arithmetics operations */
    friend Doublex2 operator +(const Doublex2 &left, const Doublex2 &right);
    friend Doublex2 operator -(const Doublex2 &left, const Doublex2 &right);

    friend Doublex2 operator +=(Doublex2 &left, const Doublex2 &right);
    friend Doublex2 operator -=(Doublex2 &left, const Doublex2 &right);

    friend Doublex2 operator *(const Doublex2 &left, const Doublex2 &right);
    friend Doublex2 operator /(const Doublex2 &left, const Doublex2 &right);

    friend Doublex2 operator *=(Doublex2 &left, const Doublex2 &right);
    friend Doublex2 operator /=(Doublex2 &left, const Doublex2 &right);

    /*Print to stream helper */
    friend ostream & operator << (ostream &out, const Doublex2 &vector);

};

FORCE_INLINE Doublex2 operator +(const Doublex2 &left, const Doublex2 &right) {
    return Doublex2(_mm_add_pd(left.data, right.data));
}

FORCE_INLINE Doublex2 operator -(const Doublex2 &left, const Doublex2 &right) {
    return Doublex2(_mm_sub_pd(left.data, right.data));
}

FORCE_INLINE Doublex2 operator +=(Doublex2 &left, const Doublex2 &right) {
    left.data = _mm_add_pd(left.data, right.data);
    return left;
}

FORCE_INLINE Doublex2 operator -=(Doublex2 &left, const Doublex2 &right) {
    left.data = _mm_sub_pd(left.data, right.data);
    return left;
}


FORCE_INLINE Doublex2 operator *(const Doublex2 &left, const Doublex2 &right) {
    return Doublex2(_mm_mul_pd(left.data, right.data));
}

FORCE_INLINE Doublex2 operator /(const Doublex2 &left, const Doublex2 &right) {
    return Doublex2(_mm_div_pd(left.data, right.data));
}

FORCE_INLINE Doublex2 operator *=(Doublex2 &left, const Doublex2 &right) {
    left.data = _mm_mul_pd(left.data, right.data);
    return left;
}

FORCE_INLINE Doublex2 operator /=(Doublex2 &left, const Doublex2 &right) {
    left.data = _mm_div_pd(left.data, right.data);
    return left;
}

/* Hide this in namespace or SSEMath */
FORCE_INLINE Doublex2 multiplyAdd(const Doublex2 &mul1, const Doublex2 &mul2, const Doublex2 &add1) {
#ifdef WITH_FMA
    return Doublex2(_mm_fmadd_pd(mul1.data, mul2.data, add1.data));
#else
    return mul1 * mul2 + add1;
#endif
}


} //namespace corecvs

#endif // DOUBLEX2_H
