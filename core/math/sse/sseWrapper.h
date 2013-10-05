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

#include "sseMath.h"
#endif //WITH_SSE


namespace corecvs {
} //namespace corecvs

#endif /* SEEWRAPPER_H_ */

