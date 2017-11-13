#ifndef DOUBLEX8_H
#define DOUBLEX8_H

/**
 * \file doublex8.h
 * \brief a header for Doublex4.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "core/utils/global.h"

#include "core/math/sse/doublex4.h"


namespace corecvs {

class ALIGN_DATA(32) Doublex8 : public Vector2d<Doublex4>
{
public:
    static const int SIZE = 8;
    static const int EL_SIZE = Doublex4::SIZE;


    Doublex8() {}

    explicit Doublex8(const Vector2d<Doublex4> &base)
    {
        this->element[0].data = base.element[0].data;
        this->element[1].data = base.element[1].data;
    }

    explicit Doublex8(const double value) {
        element[0].data = _mm256_set1_pd(value);
        element[1].data = _mm256_set1_pd(value);
    }
    /**
    *  Constructor from intrinsic type
    **/
    explicit Doublex8(const __m256d &_data) {
        element[0].data = _data;
        element[1].data = _data;
    }

    Doublex8(const __m256d &_data1, const __m256d &_data2) {
        element[0].data = _data1;
        element[1].data = _data2;
    }


    explicit Doublex8(double value1, double value2, double value3, double value4,
                      double value5, double value6, double value7, double value8)
    {
        element[0].data = _mm256_set_pd(value1, value2, value3, value4);
        element[1].data = _mm256_set_pd(value5, value6, value7, value8);
    }

    explicit Doublex8(const double * const data_ptr)
    {
        element[0].data = _mm256_loadu_pd(data_ptr    );
        element[1].data = _mm256_loadu_pd(data_ptr + 4);
    }

    inline static Doublex8 Zero()
    {
        return Doublex8(_mm256_setzero_pd());
    }

    inline static Doublex8 Broadcast(const double *data_ptr)
    {
        return Doublex8(_mm256_broadcast_sd(data_ptr));
    }

    void load(double * const data)
    {       
        this->element[0].data = _mm256_load_pd(data    );
        this->element[1].data = _mm256_load_pd(data + 4);
    }


    void save(double * const data) const
    {
        _mm256_storeu_pd(data    , this->element[0].data);
        _mm256_storeu_pd(data + 4, this->element[1].data);
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(double * const data) const
    {
        _mm256_store_pd(data    , this->element[0].data);
        _mm256_store_pd(data + 4, this->element[1].data);
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(double * const data) const
    {
        _mm256_stream_pd(data    , this->element[0].data);
        _mm256_stream_pd(data + 4, this->element[1].data);
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
    inline Doublex8 sqrt() const
    {
        return Doublex8(_mm256_sqrt_pd(this->element[0].data), _mm256_sqrt_pd(this->element[1].data));
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
            case 4: return getDouble<4>();
            case 5: return getDouble<5>();
            case 6: return getDouble<6>();
            case 7: return getDouble<7>();
        }
        return 0;
    }

    /*Print to stream helper */
    friend ostream & operator << (ostream &out, const Doublex4 &vector);


    /* Formated hex helper */
    void hexDump (ostream &out);
};


FORCE_INLINE Doublex8 multiplyAdd(const Doublex8 &mul1, const Doublex8 &mul2, const Doublex8 &add1) {
#ifdef WITH_FMA
    return Doublex8(
        _mm256_fmadd_pd(mul1.element[0].data, mul2.element[0].data, add1.element[0].data),
        _mm256_fmadd_pd(mul1.element[1].data, mul2.element[1].data, add1.element[1].data)
    );
#else
    return Doublex8(mul1 * mul2 + add1);
#endif
}



} //namespace corecvs

#endif // DOUBLEX8_H
