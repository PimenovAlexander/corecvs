#ifndef FLOATT4_H
#define FLOATT4_H

/**
 * \file floatT4.h
 * \brief a header for Float4.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"
#include "core/math/avx/float32x8.h"


namespace corecvs {

template<int ELEMENTS>
class ALIGN_DATA(32) FloatT8 : public FixedVectorBase<FloatT8<ELEMENTS>, Float32x8, ELEMENTS>
{
public:
    static const int SIZE = Float32x8::SIZE * ELEMENTS;
    static const int EL_SIZE = Float32x8::SIZE;


    FloatT8() {}

#if 0
    explicit DoublexT4(const FixedVector<Doublex4, ELEMENTS> &base)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = base.element[r].data;
        }
    }
#endif

    explicit FloatT8(const float value) {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_set1_ps(value);
        }
    }
    /**
    *  Constructor from intrinsic type
    **/
    explicit FloatT8(const __m256 &_data) {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _data;
        }
    }

    explicit FloatT8(const float * const data_ptr)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_loadu_ps(data_ptr + EL_SIZE * r);
        }
    }

    inline static FloatT8 Zero()
    {
        return FloatT8(_mm256_setzero_ps());
    }

    inline static FloatT8 Broadcast(const float *data_ptr)
    {
        return FloatT8(_mm256_broadcast_ss(data_ptr));
    }

    void load(float * const data)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_load_ps(data + EL_SIZE * r);
        }
    }


    void save(float * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_storeu_ps(data + EL_SIZE * r, this->element[r].data);
        }
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(float * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_store_ps(data + EL_SIZE * r, this->element[0].data);
        }
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(float * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_stream_ps(data + EL_SIZE * r, this->element[0].data);
        }
    }

    /*Print to stream helper */
    // friend ostream & operator << (ostream &out, const FloatT8 &vector);


    /* Formated hex helper */
    void hexDump (ostream &out);
};

template<int ELEMENTS>
FORCE_INLINE FloatT8<ELEMENTS> multiplyAdd(const FloatT8<ELEMENTS> &mul1, const FloatT8<ELEMENTS> &mul2, const FloatT8<ELEMENTS> &add1) {
#ifdef WITH_FMA
    FloatT8<ELEMENTS> result;

    for (int r = 0; r < ELEMENTS; r++) {
        result[r].data  = _mm256_fmadd_ps(mul1.element[r].data, mul2.element[r].data, add1.element[r].data);
    }
    return result;
#else
    return FloatT8<ELEMENTS>(mul1 * mul2 + add1);
#endif
}



} //namespace corecvs

#endif // FLOAT4_H
