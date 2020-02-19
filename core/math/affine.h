/**
 * \file affine.h
 * \brief Add Comment Here
 *
 * \date Apr 24, 2011
 * \author alexander
 */
#ifndef AFFINE_H_
#define AFFINE_H_

#include "math/vector/vector3d.h"
#include "math/quaternion.h"
#include "math/eulerAngles.h"
#include "math/matrix/matrix33.h"
#include "math/matrix/matrix22.h"
#include "geometry/line.h"

namespace corecvs {

/**
 *  \brief Template class that holds the affine transformation of the space in form of rotational/scale/shear part
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

    explicit Affine3D(LinearType _rotor = LinearType::Identity(), Vector3dd _shift = Vector3dd::Zero())
        : rotor(_rotor)
        , shift(_shift)
    {}

    explicit Affine3D(Vector3dd _shift)
        : rotor(LinearType::Identity())
        , shift(_shift)
    {}

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

    friend inline Ray3d operator *(const Affine3D &affine, const Ray3d &r)
    {
        return Ray3d(affine.rotor * r.a, affine * r.p);
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

    /**
     *   This is an utility method. If would work correctly if top 3x3 matrix is unitary
     **/
    static Affine3D FromMatrix44(const Matrix44 &m) {
        return Affine3D(Quaternion::FromMatrix(m.topLeft33()), m.translationPart());
    }

    Affine3D inverted() const;

    explicit operator corecvs::Matrix44() const;

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(shift, Vector3dd(0.0, 0.0, 0.0) , "shift");
        visitor.visit(rotor, Quaternion::Identity()   , "rotor");
    }

    /* Pretty print */
    //void prettyPrint (ostream &out = cout) const;
      void prettyPrint1(std::ostream &out = std::cout) const;
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

template <>
corecvs::Affine3D<Matrix33>  ::operator corecvs::Matrix44() const;
template <>
corecvs::Affine3D<Quaternion>::operator corecvs::Matrix44() const;



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

typedef Affine3D<Quaternion>  Affine3DQ;
typedef Affine3D<Matrix33>    Affine3DM;
typedef Affine3D<EulerAngles> Affine3DE;



/**
 * Affine 2d
   TODO: Merge common code in CRTP style
**/

template <typename LinearType>
class Affine2D
{
public:
    LinearType rotor;
    Vector2dd  shift;

    explicit Affine2D(LinearType _rotor = LinearType::Identity(), Vector2dd _shift = Vector2dd::Zero())
        : rotor(_rotor)
        , shift(_shift)
    {}

    explicit Affine2D(Vector2dd _shift)
        : rotor(LinearType::Identity())
        , shift(_shift)
    {}

    static LinearType superpose(const LinearType &l1, const LinearType &l2);

    friend std::ostream& operator<< (std::ostream& os, const Affine2D &t)
    {
        os << t.shift << " " << t.rotor;
        return os;
    }

    friend inline Vector2dd operator *(const Affine2D &affine, const Vector2dd &x)
    {
        return affine.rotor * x + affine.shift;
    }

    friend inline Ray2d operator *(const Affine2D &affine, const Ray2d &r)
    {
        return Ray2d(affine.rotor * r.a, affine * r.p);
    }

    inline Vector2dd apply(const Vector2dd &x) const
    {
        return (*this) * x;
    }

    //inline Vector2dd applyInv(const Vector2dd &x) const;

    /**
     *
     *   A2 := ax + b
     *   A1 := cx + d
     *
     *   A1 * A2 := c(ax + b) + d = ca(x) +  (cb + d)
     *
     **/
    friend inline Affine2D operator *(const Affine2D &A1, const Affine2D &A2)
    {
        return Affine2D(
             superpose(A1.rotor, A2.rotor),
             A1.rotor * A2.shift + A1.shift
        );
    }

    static Affine2D Rotation(double angle)
    {
        return Affine2D(LinearType::RotationX(angle));
    }

    static Affine2D Identity()
    {
        return Affine2D(LinearType::Identity());
    }

    static Affine2D Shift(const Vector3dd &translation)
    {
        return Affine2D(LinearType::Identity(), translation);
    }

    static Affine2D Shift(double x, double y, double z)
    {
        return Affine2D(LinearType::Identity(), Vector3dd(x, y, z));
    }

    //Affine2D inverted() const;

    //explicit operator corecvs::Matrix33() const;

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(shift, Vector3dd(0.0, 0.0, 0.0) , "shift");
        visitor.visit(rotor, Quaternion::Identity()   , "rotor");
    }

    /* Pretty print */
    //void prettyPrint (ostream &out = cout) const;
    //void prettyPrint1(ostream &out = cout) const;
};

typedef Affine2D<Matrix22>  Affine2DM;


} // namespace corecvs

#endif /* AFFINE_H_ */
