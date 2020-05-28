/**
 * \file sseWrapper.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Sep 24, 2010
 * \author alexander
 */

#ifndef NEON_WRAPPER_H_
#define NEON_WRAPPER_H_

#ifdef WITH_NEON
#include "math/neon/int64x2.h"
#include "math/neon/int32x4.h"
#include "math/neon/uInt32x4.h"
//#include "math/neon/int32x8.h"
#include "math/neon/int32x8v.h"

// Int32x8 is a vector of two Int32x4

namespace corecvs {
typedef Int32x8v Int32x8;
}

#include "math/neon/int16x8.h"
#include "math/neon/uInt16x8.h"


#include "math/neon/float32x4.h"

namespace corecvs {
typedef Float32x4  FloatxN;

typedef Int32x4  Int32xN;
typedef Int16x8  Int16xN;

} //namespace corecvs


#include "neonMath.h"


#endif //WITH_NEON

#endif /* NEON_WRAPPER_H_ */

