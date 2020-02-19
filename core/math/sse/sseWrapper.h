/**
 * \file sseWrapper.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 24, 2010
 * \author alexander
 */

#ifndef SEEWRAPPER_H_
#define SEEWRAPPER_H_

#ifdef WITH_SSE
#include "math/sse/sseInteger.h"
#include "math/sse/int64x2.h"
#include "math/sse/int32x4.h"
#include "math/sse/int32x8v.h"
#include "math/sse/int16x8.h"
#include "math/sse/uInt16x8.h"

#include "math/sse/int8x16.h"
#include "math/sse/uInt8x16.h"

#include "math/sse/float32x4.h"
#include "math/sse/doublex2.h"
#endif // WITH_SSE

#ifdef WITH_AVX
#include "math/sse/float32x8.h"
#include "math/sse/doublex4.h"
#include "math/sse/doublex8.h"
#include "math/sse/doublexT4.h"
#include "math/sse/floatT8.h"
#endif // WITH_AVX

#ifdef WITH_AVX2
#include "math/avx/avxInteger.h"
#include "math/avx/int64x4.h"
#include "math/avx/int32x8.h"
#include "math/avx/int32x16v.h"
#include "math/avx/int16x16.h"
#endif // WITH_AVX2

#ifdef WITH_SSE
#include "math/sse/sseMath.h"
#endif

#ifdef _MSC_VER
# ifdef WITH_SSE4_2
#  include <intrin.h>
# endif
#endif

namespace corecvs {

#ifdef _MSC_VER
# ifdef WITH_SSE4_2
    //#  include <intrin.h> // must be above
#  define __builtin_popcount __popcnt
# else
#  define __builtin_popcount popcount_lookup
    unsigned int popcount_lookup(unsigned int a);
# endif
#else
    // gcc 5.3 correctly manages the function "__builtin_popcount" depend on native mode for CPU during compiling for any kind of CPU. Check this!?
#endif

#ifdef WITH_AVX2
    typedef Doublex4 DoublexN;
    typedef Int32x8   Int32xN;
    typedef Int16x16  Int16xN;
#elif WITH_AVX
    typedef Doublex4 DoublexN;
    typedef Int32x4  Int32xN;
    typedef Int16x8  Int16xN;
#elif WITH_SSE
    typedef Doublex2 DoublexN;
    typedef Int32x4  Int32xN;
    typedef Int16x8  Int16xN;
#endif

} //namespace corecvs

#endif /* SEEWRAPPER_H_ */
