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

#ifdef WITH_NEON
#include "core/math/neon/int64x2.h"
#include "core/math/neon/int32x4.h"
#include "core/math/neon/int32x8.h"
#include "core/math/neon/int16x8.h"


#include "core/math/neon/float32x4.h"

#include "ssemath.h"

#endif //WITH_NEON

namespace corecvs {

} //namespace corecvs
#endif /* SEEWRAPPER_H_ */

