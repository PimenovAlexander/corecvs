#ifndef INT64X2_H_
#define INT64X2_H_
/**
 * \file int64x2.h
 * \brief
 *
 * \ingroup cppcorefiles
 * \date Sep 25, 2010
 * \author: alexander
 */

#include <arm_neon.h>
#include <stdint.h>

#include "core/utils/global.h"
#include "core/math/vector/fixedVector.h"
namespace corecvs {

class ALIGN_DATA(16) Int64x2
{
public:
    /**
     *  Handle to SSE 128 value.
     *
     *  NB Union here dramatically decrease speed. So don't use one
     **/

    int64x2_t data;

    /* Constructors */
    Int64x2(){};
    /**
     *  Copy constructor
     **/
    Int64x2(const Int64x2 &other) {
        this->data = other.data;
    }

    /**
     *  Create SSE integer vector from integer constant
     **/
    Int64x2(const int64x2_t &_data) {
        this->data = _data;
    }

    Int64x2(int64_t constant) {
        this->data = vdupq_n_s64(constant);
    }

    /*
    Int64x2(int64_t c0, int64_t c1) {
        this->data = _mm_set_epi64(c0, c1);
    }*/

    Int64x2(const int64_t data[2]) {
        this->data = vld1q_s64(&data[0]);
    }

    Int64x2(const FixedVector<int64_t,2> input)  {
        this->data = vld1q_s64(&input.element[0]);
    }


    /* Static fabrics */
    /** Load unaligned. Not safe to use until you exactly know what you are doing */

    static Int64x2 load(const int64_t data[4])
    {
        return Int64x2(data);
    }

    /** Load aligned. Not safe to use untill you exactly know what you are doing */
    static Int64x2 loadAligned(const int64_t data[4])
    {
        return vld1q_s64(&data[0]);
    }

    /* Savers */
    void save(int64_t data[4])
    {
        vst1q_s64(&data[0], this->data);
    }

    void save(uint64_t data[4])
    {
       vst1q_s64((int64_t*)&data[0], this->data);
    }

    /** Save aligned. Not safe to use untill you exactly know what you are doing */
    void saveAligned(int64_t data[4])
    {
        vst1q_s64(&data[0], this->data);
    }

    void saveAligned(uint64_t data[4])
    {
        vst1q_s64((int64_t*)&data[0], this->data);
    }


    /** Stream aligned. Not safe to use untill you exactly know what you are doing */
    void streamAligned(int64_t data[4])
    {
        vst1q_s64(&data[0], this->data);
    }

    /* converters */
    uint64_t getInt(uint32_t idx)
    {
        ASSERT_TRUE(idx < 2, "Wrong idx in getInt()");
        ALIGN_DATA(16) int64_t result[2];
        saveAligned(result);
        return result[idx];
    }
};


} //namespace corecvs
#endif  //INT64X2_H_

