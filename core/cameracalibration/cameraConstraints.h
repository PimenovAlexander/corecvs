#ifndef CAMERACONSTRAINTS_H
#define CAMERACONSTRAINTS_H

#include <type_traits>

#include "utils/typesafeBitmaskEnums.h"

namespace corecvs {
enum class CameraConstraints
{
    NONE              =  0x00,
    ZERO_SKEW         =  0x01, // This forces skew to zero, but not locks it to zero during non-linear optimization
    LOCK_SKEW         =  0x02, // This one locks skew, but not forces it to zero
    EQUAL_FOCAL       =  0x04, // Makes fx = fy in non-linear phase
    LOCK_FOCAL        =  0x08, // Locks fx and fy
    LOCK_PRINCIPAL    =  0x10, // Locks cx and cy
    UNLOCK_YSCALE     =  0x20, // Unlock Y scale of pattern. This is dangerous if you are not sure what are you doing
    UNLOCK_DISTORTION =  0x40, // Allow estimation of distortion parameters

    DEFAULT           = (ZERO_SKEW | LOCK_SKEW | EQUAL_FOCAL),  // default set of constraints for any our camera
    // Not used now
    LOCK_PRINCIPALS   =  0x80  // Force equivalence of distortion and projective principal points (works only with UNLOCK_DISTORTION)
};


template<>
struct is_bitmask<corecvs::CameraConstraints> : std::true_type {};

} // namespace corecvs

#endif // CAMERACONSTRAINTS_H
