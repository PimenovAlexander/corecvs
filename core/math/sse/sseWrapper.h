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
#include "int32x8.h"
#include "int16x8.h"
#include "uInt16x8.h"

#include "int8x16.h"
#include "uInt8x16.h"

#include "float32x4.h"
#include "doublex2.h"

#include "sseMath.h"

#endif //WITH_SSE

#ifdef WITH_AVX
#include "doublex4.h"
#include "doublex8.h"
#include "doublexT4.h"
#endif

namespace corecvs {

#ifdef WITH_AVX
typedef Doublex4 DoublexN;
#else
#   ifdef WITH_SSE
typedef Doublex2 DoublexN;
#   endif
#endif

} //namespace corecvs

#endif /* SEEWRAPPER_H_ */

