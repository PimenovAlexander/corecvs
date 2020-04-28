/**
 * \file affine.cpp
 * \brief Add Comment Here
 *
 * \date Apr 24, 2011
 * \author alexander
 */
#include "math/affine.h"
#include "math/mathUtils.h"
#include "math/matrix/matrix44.h"

namespace corecvs {

template<>
void Affine3DQ::prettyPrint1(std::ostream &out)  const
{
    Quaternion o = rotor.normalised();
    Vector3dd axis = o.getAxis();
    double   angle = radToDeg(o.getAngle());

    out << "Pos:" <<  shift << std::endl;
    out << "Rotation around normalized axis: " << axis << " angle " << angle << "deg" << std::endl;
}

template <>
corecvs::Affine3D<Matrix33>::operator corecvs::Matrix44() const
{
    return corecvs::Matrix44::Shift(shift) * corecvs::Matrix44(rotor);
}

template <>
corecvs::Affine3D<Quaternion>::operator corecvs::Matrix44() const
{
    return corecvs::Matrix44::Shift(shift) * corecvs::Matrix44(rotor.toMatrix());
}

} //namespace corecvs
