#ifndef DOUBLEXT4_H
#define DOUBLEXT4_H

/**
 * \file doublexT4.h
 * \brief a header for Doublex4.cpp
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include "global.h"

#include "fixedVector.h"
#include "doublex4.h"


namespace corecvs {

template<int ELEMENTS>
class ALIGN_DATA(32) DoublexT4 : public FixedVectorBase<DoublexT4<ELEMENTS>, Doublex4, ELEMENTS>
{
public:
    static const int SIZE = Doublex4::SIZE * ELEMENTS;
    static const int EL_SIZE = Doublex4::SIZE;


    DoublexT4() {}

#if 0
    explicit DoublexT4(const FixedVector<Doublex4, ELEMENTS> &base)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = base.element[r].data;
        }
    }
#endif

    explicit DoublexT4(const double value) {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_set1_pd(value);
        }
    }
    /**
    *  Constructor from intrinsic type
    **/
    explicit DoublexT4(const __m256d &_data) {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _data;
        }
    }

    explicit DoublexT4(const double * const data_ptr)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_loadu_pd(data_ptr + EL_SIZE * r);
        }
    }

    inline static DoublexT4 Zero()
    {
        return DoublexT4(_mm256_setzero_pd());
    }

    inline static DoublexT4 Broadcast(const double *data_ptr)
    {
        return DoublexT4(_mm256_broadcast_sd(data_ptr));
    }

    void load(double * const data)
    {
        for (int r = 0; r < ELEMENTS; r++) {
            this->element[r].data = _mm256_load_pd(data + EL_SIZE * r);
        }
    }


    void save(double * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_storeu_pd(data + EL_SIZE * r, this->element[r].data);
        }
    }

    /** Save aligned.
     * \remark Not safe to use until you exactly know what you are doing
     **/
    void saveAligned(double * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_store_pd(data + EL_SIZE * r, this->element[0].data);
        }
    }

    /**
     * Stream aligned.
     * \remark Not safe to use until you exactly know what you are doing
     * */
    void streamAligned(double * const data) const
    {
        for (int r = 0; r < ELEMENTS; r++) {
            _mm256_stream_pd(data + EL_SIZE * r, this->element[0].data);
        }
    }

    /*Print to stream helper */
    //friend ostream & operator << (ostream &out, const Doublex4 &vector);


    /* Formated hex helper */
    void hexDump (ostream &out);
};

template<int ELEMENTS>
FORCE_INLINE DoublexT4<ELEMENTS> multiplyAdd(const DoublexT4<ELEMENTS> &mul1, const DoublexT4<ELEMENTS> &mul2, const DoublexT4<ELEMENTS> &add1) {
#ifdef WITH_FMA
    DoublexT4<ELEMENTS> result;

    for (int r = 0; r < ELEMENTS; r++) {
        result[r].data  = _mm256_fmadd_pd(mul1.element[r].data, mul2.element[r].data, add1.element[r].data);
    }
    return result;
#else
    return DoublexT4<ELEMENTS>(mul1 * mul2 + add1);
#endif
}



} //namespace corecvs

#endif // DOUBLEXT4_H
