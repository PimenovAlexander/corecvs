/**
 * \file affine.h
 * \brief Add Comment Here
 *
 * \date Apr 24, 2011
 * \author alexander
 */

#ifndef AFFINE_H_
#define AFFINE_H_
#include "vector3d.h"
#include "quaternion.h"
#include "matrix33.h"
namespace corecvs {


/**
 *  \brief Template class that holds the affine transformation of the space in form of rotational/shear part
 *  and shift part.
 *
 *  Generally it is either quaternion based (and can't hold shear/scale on this case) or Matrix33 based.
 *
 *
 **/
template <typename LinearType>
class Affine3D
{
public:
    LinearType rotor;
    Vector3dd   shift;


    Affine3D(const LinearType &_rotor, const Vector3dd &_shift = Vector3dd(0.0)) :
        rotor(_rotor),
        shift(_shift)
    {};

    friend inline Vector3dd operator *(const Affine3D &affine, const Vector3dd &x)
    {
        return affine.rotor * x + affine.shift;
    }

    /**
     *
     *   A2 := ax + b
     *   A1 := cx + d
     *
     *   A1 * A2 := c(ax + b) + d = ca(x) +  (cb + d)
     *
     **/
    friend inline Affine3D operator *(const Affine3D &A1, const Affine3D &A2)
    {
        return Affine3D(
             A1.rotor * A2.rotor,
             A1.rotor * A2.shift + A1.shift
        );
    }

    static Affine3D RotationX(double angle)
    {
        return LinearType::RotationX(angle);
    }

    static Affine3D RotationY(double angle)
    {
        return LinearType::RotationY(angle);
    }

    static Affine3D RotationZ(double angle)
    {
        return LinearType::RotationZ(angle);
    }


};

typedef Affine3D<Quaternion> Affine3DQ;
typedef Affine3D<Matrix33>   Affine3DM;


/*Affine 2d*/


} // namespace corecvs
#endif /* AFFINE_H_ */

