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
#include "sseInteger.h"
#include "int64x2.h"
#include "int32x4.h"
#include "int32x8v.h"
#include "int16x8.h"
#include "uInt16x8.h"

#include "int8x16.h"
#include "uInt8x16.h"

#include "float32x4.h"
#include "doublex2.h"
#endif // WITH_SSE

#ifdef WITH_AVX
#include "doublex4.h"
#include "doublex8.h"
#include "doublexT4.h"
#include "floatT8.h"
#endif // WITH_AVX

#ifdef WITH_AVX2
#include "avxInteger.h"
#include "int64x4.h"
#include "int32x8.h"
#include "int16x16.h"
#endif // WITH_AVX2

#ifdef WITH_SSE
#include "sseMath.h"
#endif

namespace corecvs {

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
