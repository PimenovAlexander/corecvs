#ifndef SSE_TRACE_H_
#define SSE_TRACE_H_
/**
 * \file sse_trace.h
 * \brief a header for sse_trace.c
 *
 * \ingroup cppcorefiles
 * \date Sep 22, 2010
 * \author: alexander
 */

#ifdef WITH_SSE

#include <stdint.h>
#include <emmintrin.h>

namespace corecvs {

inline uint32_t sse32(__m128i sse, int index)
{
    ALIGN_DATA(16) uint32_t dump[4];
    _mm_store_si128((__m128i*)dump, sse);
    return dump[index];
}

inline uint16_t sse16(__m128i sse, int index)
{
    ALIGN_DATA(16) uint16_t dump[8];
    _mm_store_si128((__m128i*)dump, sse);
    return dump[index];
}

} //namespace corecvs

#endif // WITH_SSE

#endif  //SSE_TRACE_H_

