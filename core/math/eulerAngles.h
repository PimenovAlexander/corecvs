/**
* \file eulerAngles.h
* \brief a header for eulerAngles.c
*
* \date Feb 9, 2011
* \author: alexander
*/
#ifndef EULERANGLES_H_
#define EULERANGLES_H_

#include "core/math/quaternion.h"

namespace corecvs {

/**
 * Inside the project angles are in radians
 *
 **/
class EulerAngles
{
public:
    double alpha; /**< Rotation around X axis */
    double beta;  /**< Rotation around Y axis */
    double gamma; /**< Rotation around Z axis */

    EulerAngles() : alpha(0.0), beta(0.0), gamma(0.0) {}

    EulerAngles(
        double _alpha,
        double _beta,
        double _gamma)
    : alpha(_alpha)
    , beta (_beta)
    , gamma(_gamma)
    {}

    friend std::ostream & operator <<(std::ostream &out, const EulerAngles &angels)
    {
        out << "[alpha=" << angels.alpha << ", beta=" << angels.beta <<  ", gamma=" << angels.gamma  << "]";
        return out;
    }
};


/**
 *  This should be merged with Affine class
 **/
class ShiftRotateTransformation
{
public:
    EulerAngles rotation;
    Vector3dd   translation;

    ShiftRotateTransformation() :
        rotation(0.0,0.0,0.0),
        translation(0.0)
    {}


    ShiftRotateTransformation(
            const EulerAngles &_rotation,
            const Vector3dd   &_translation
    ) :
        rotation(_rotation),
        translation(_translation)
    {}
};


} //namespace corecvs

#endif  //EULERANGLES_H_
