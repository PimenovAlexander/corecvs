/**
 * \file affine.cpp
 * \brief Add Comment Here
 *
 * \date Apr 24, 2011
 * \author alexander
 */

#include "affine.h"
#include "mathUtils.h"

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


} //namespace corecvs

