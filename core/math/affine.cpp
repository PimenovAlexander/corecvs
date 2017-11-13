/**
 * \file affine.cpp
 * \brief Add Comment Here
 *
 * \date Apr 24, 2011
 * \author alexander
 */

#include "core/math/affine.h"
#include "core/math/mathUtils.h"

namespace corecvs {

template<>
void Affine3DQ::prettyPrint1(std::ostream &out)  const
{
    Quaternion o = rotor.normalised();
    Vector3dd axis = o.getAxis();
    double   angle = radToDeg(o.getAngle());

    out << "Pos:" <<  shift << std::endl;
    out << "Rotation around: " << axis << " angle " << angle << "deg" << std::endl;
}

template <>
corecvs::Affine3D<Matrix33>::operator corecvs::Matrix44() const
{
    return corecvs::Matrix44::Shift(shift[0], shift[1], shift[2]) * corecvs::Matrix44(rotor);
}
template <>
corecvs::Affine3D<Quaternion>::operator corecvs::Matrix44() const
{
    return corecvs::Matrix44::Shift(shift[0], shift[1], shift[2]) * corecvs::Matrix44(rotor.toMatrix());
}


} //namespace corecvs

