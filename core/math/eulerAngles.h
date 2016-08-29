#ifndef EULERANGLES_H_
#define EULERANGLES_H_
#include "quaternion.h"
namespace corecvs {
/**
 * \file eulerAngles.h
 * \brief a header for eulerAngles.c
 *
 * \date Feb 9, 2011
 * \author: alexander
 */



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

    friend ostream & operator <<(ostream &out, const EulerAngles &angels)
    {
        out << "[alpha=" << angels.alpha << ", beta=" << angels.beta <<  ", gamma=" << angels.gamma  << "]";
        return out;
    }

};


/**
 *  As rotation depends on the order of application of the rotations of Euler angles
 *  this class specifies one of possible orders that will be used for camera rotation description
 *
 *
 *   <ul>
 *   <li>\f$OX\f$ -to the right of the image (from top left to the top right corner)</li>
 *   <li>\f$OY\f$ -downwards (from top left to the bottom left corner)</li>
 *   <li>\f$OZ\f$ -orthogonal, goes from the camera into the scene</li>
 *   </ul>
 *
 *   <pre>
 *          ^ Z
 *         /
 *        /
 *       /
 *      /
 *     O----------------->  X
 *     |
 *     |
 *     |
 *     |
 *     |
 *     v  Y
 *
 *   </pre>
 *
 *

 *
 **/
class CameraAnglesLegacy : public EulerAngles
{
public:
    CameraAnglesLegacy() {}

    CameraAnglesLegacy(double pitch, double yaw, double roll) :
        EulerAngles(pitch, yaw, roll)
    {}

    double  pitch() const
    {
        return alpha;
    }

    void setPitch(double pitch)
    {
        alpha = pitch;
    }

    double  yaw() const
    {
        return beta;
    }

    void setYaw(double yaw)
    {
        beta = yaw;
    }

    double  roll() const
    {
        return gamma;
    }

    void setRoll(double roll)
    {
        gamma = roll;
    }

    Matrix33 toMatrix() const
    {
        return
            Matrix33::RotationZ(roll()) *
            Matrix33::RotationY(yaw()) *
            Matrix33::RotationX(pitch());
    }

    /**
     *  \f[
     *  \pmatrix{ \phi \cr \theta \cr \psi } =
     *
     *  \pmatrix{
     *     atan2  (2(q_0 q_1 + q_2 q_3),1 - 2(q_1^2 + q_2^2)) \cr
     *     arcsin (2(q_0 q_2 - q_3 q_1)) \cr
     *     atan2  (2(q_0 q_3 + q_1 q_2),1 - 2(q_2^2 + q_3^2))
     *  }
     *
     *  \f]
     *
     */
    static CameraAnglesLegacy FromQuaternion(Quaternion &Q)
    {
        double pitch = atan2 (2.0 * (Q.t() * Q.x() + Q.y() * Q.z()),1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y()));
        double yaw   = asin  (2.0 * (Q.t() * Q.y() - Q.z() * Q.x()));
        double roll  = atan2 (2.0 * (Q.t() * Q.z() + Q.x() * Q.y()),1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z()));
        return CameraAnglesLegacy(pitch, yaw, roll);
    }


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(alpha, 0.0, "pitch");
        visitor.visit(beta , 0.0, "yaw"  );
        visitor.visit(gamma, 0.0, "roll" );
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

