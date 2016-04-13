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
    Vector3dd  shift;

    explicit Affine3D(
            LinearType _rotor = LinearType::Identity(),
            Vector3dd  _shift = Vector3dd(0.0, 0.0, 0.0)
           ) :
        rotor(_rotor),
        shift(_shift)
    {
    }

    explicit Affine3D(Vector3dd  _shift) :
        rotor(LinearType::Identity()),
        shift(_shift)
    {
    }

    static LinearType superpose(const LinearType &l1, const LinearType &l2);

    friend std::ostream& operator<< (std::ostream& os, const Affine3D &t)
    {
        os << t.shift << " " << t.rotor;
        return os;
    }

    friend inline Vector3dd operator *(const Affine3D &affine, const Vector3dd &x)
    {
        return affine.rotor * x + affine.shift;
    }

    inline Vector3dd apply(const Vector3dd &x) const
    {
        return (*this) * x;
    }

    inline Vector3dd applyInv(const Vector3dd &x) const;

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
             superpose(A1.rotor, A2.rotor),
             A1.rotor * A2.shift + A1.shift
        );
    }

    static Affine3D RotationX(double angle)
    {
        return Affine3D(LinearType::RotationX(angle));
    }

    static Affine3D RotationY(double angle)
    {
        return Affine3D(LinearType::RotationY(angle));
    }

    static Affine3D RotationZ(double angle)
    {
        return Affine3D(LinearType::RotationZ(angle));
    }

    static Affine3D Identity()
    {
        return Affine3D(LinearType::Identity());
    }

    static Affine3D Shift(const Vector3dd &translation)
    {
        return Affine3D(LinearType::Identity(), translation);
    }

    static Affine3D Shift(double x, double y, double z)
    {
        return Affine3D(LinearType::Identity(), Vector3dd(x, y, z));
    }

    Affine3D inverted() const;

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(shift, Vector3dd(0.0, 0.0, 0.0) , "shift");
        visitor.visit(rotor, Quaternion::Identity()   , "rotor");
    }

    /* Pretty print */
    void prettyPrint (ostream &out = cout) const;
    void prettyPrint1(ostream &out = cout) const;

};

/**
 *     R * X + T = Y
 *     X = R^{-1} * (Y - T) = R^{-1} * Y + (- R^{-1} * T)
 **/
template<>
inline Affine3D<Quaternion> Affine3D<Quaternion>::inverted() const
{
    Quaternion inv = this->rotor.conjugated();
    return Affine3D<Quaternion>(inv,  - (inv * this->shift));
}

template<>
inline Affine3D<Matrix33> Affine3D<Matrix33>::inverted() const
{
    Matrix33 inv = this->rotor.inv();
    return Affine3D<Matrix33>(inv,  - (inv * this->shift));
}

/**/
template<>
inline Quaternion Affine3D<Quaternion>::superpose(const Quaternion &l1, const Quaternion &l2)
{
    return l1 ^ l2;
}

template<>
inline Matrix33 Affine3D<Matrix33>::superpose(const Matrix33  &l1, const Matrix33 &l2)
{
    return l1 * l2;
}

template<>
inline Vector3dd Affine3D<Quaternion>::applyInv(const Vector3dd &x) const
{
    return rotor.conjugated() * (x - shift);
}

typedef Affine3D<Quaternion> Affine3DQ;
typedef Affine3D<Matrix33>   Affine3DM;


/*Affine 2d*/


} // namespace corecvs
#endif /* AFFINE_H_ */

