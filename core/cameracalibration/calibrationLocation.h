#ifndef CALIBRATION_LOCATION_H
#define CALIBRATION_LOCATION_H

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/math/quaternion.h"
#include "core/math/matrix/matrix44.h"
#include "core/geometry/line.h"
#include "core/math/eulerAngles.h"
#include "core/math/affine.h"
#include "core/reflection/printerVisitor.h"

#include "core/math/mathUtils.h"

namespace corecvs {

/**
 *    The classical rotation storage is in format yaw, pitch and roll.
 *    This class works in CAMERA reference frame with
 *
 *     X - horisontaly to the image right
 *     Y - downwards
 *     Z - following optical axis from camera, forward
 *
 *
 *    Yaw/Athimuth [0..2pi]
 *    Pitch
 *    Roll
 **/
class CameraLocationAngles : public EulerAngles
{
public:
    CameraLocationAngles(double yaw = 0.0, double pitch = 0.0, double roll = 0.0) :
        EulerAngles(yaw, pitch, roll)
    {}

    static CameraLocationAngles FromAnglesDeg(double yawDeg, double pitchDeg, double rollDeg)
    {
        return CameraLocationAngles(degToRad(yawDeg), degToRad(pitchDeg), degToRad(rollDeg));
    }

    double  yaw() const
    {
        return alpha;
    }

    void setYaw(double yaw)
    {
        alpha = yaw;
    }

    double  pitch() const
    {
        return beta;
    }

    void setPitch(double pitch)
    {
        beta = pitch;
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

    Quaternion toQuaternion() const
    {
        return
            Quaternion::RotationZ(roll()) ^
            Quaternion::RotationY(yaw()) ^
            Quaternion::RotationX(pitch());
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
    static CameraLocationAngles FromQuaternion(const Quaternion &Q)
    {
        double yaw   = asin  (2.0 * (Q.t() * Q.y() - Q.z() * Q.x()));
        double pitch = atan2 (2.0 * (Q.t() * Q.x() + Q.y() * Q.z()),1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y()));
        double roll  = atan2 (2.0 * (Q.t() * Q.z() + Q.x() * Q.y()),1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z()));
        return CameraLocationAngles(yaw, pitch, roll);
    }


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(alpha, 0.0, "yaw"  );
        visitor.visit(beta , 0.0, "pitch");
        visitor.visit(gamma, 0.0, "roll" );
    }

    CameraLocationAngles toDeg() const {
        return CameraLocationAngles(
                    radToDeg(yaw()),
                    radToDeg(pitch()),
                    radToDeg(roll())
                    );
    }

    CameraLocationAngles toRad() const {
        return CameraLocationAngles(
                    degToRad(yaw()),
                    degToRad(pitch()),
                    degToRad(roll())
                    );
    }

    friend ostream& operator << (ostream &out, CameraLocationAngles &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void prettyPrint (ostream &out = cout);

};



/**
 *    The classical rotation storage is in format yaw, pitch and roll.
 *    This class works in WORLD reference frame with
 *
 *     X - to the North
 *     Y - to the East
 *     Z - to the sky
 *
 *
 *    Yaw/Athimuth [0..2pi]
 *    Pitch
 *    Roll
 *
 *    Order of application while rotating the object is Yaw first, Pitch, Roll last
 *
 **/
class WorldLocationAngles : public EulerAngles
{
public:
    WorldLocationAngles(double yaw = 0.0, double pitch = 0.0, double roll = 0.0) :
        EulerAngles(roll, pitch, yaw)
    {}

    static WorldLocationAngles FromAngles(double yawDeg, double pitchDeg, double rollDeg)
    {
        return WorldLocationAngles(degToRad(yawDeg), degToRad(pitchDeg), degToRad(rollDeg));
    }

    double  yaw() const
    {
        return gamma;
    }

    void setYaw(double yaw)
    {
        gamma = yaw;
    }

    double  pitch() const
    {
        return beta;
    }

    void setPitch(double pitch)
    {
        beta = pitch;
    }

    double  roll() const
    {
        return alpha;
    }

    void setRoll(double roll)
    {
        alpha = roll;
    }

    Matrix33 toMatrix() const
    {
        return
            Matrix33::RotationZ(yaw()) *
            Matrix33::RotationY(pitch()) *
            Matrix33::RotationX(roll());
    }

    Quaternion toQuaternion() const
    {
        return
            Quaternion::RotationZ(yaw()) ^
            Quaternion::RotationY(pitch()) ^
            Quaternion::RotationX(roll());
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
    static WorldLocationAngles FromQuaternion(const Quaternion &Q)
    {
        /*
        double roll  = asin  (2.0 * (Q.t() * Q.y() - Q.z() * Q.x()));
        double pitch = atan2 (2.0 * (Q.t() * Q.x() + Q.y() * Q.z()),1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y()));
        double yaw   = atan2 (2.0 * (Q.t() * Q.z() + Q.x() * Q.y()),1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z()));
        */

        double roll, pitch, yaw;
        // roll (x-axis rotation)
        double sinr = +2.0 * (Q.t() * Q.x() + Q.y() * Q.z());
        double cosr = +1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y());
        roll = atan2(sinr, cosr);

        // pitch (y-axis rotation)
        double sinp = +2.0 * (Q.t() * Q.y() - Q.z() * Q.x());
            if (fabs(sinp) >= 1)
                pitch = copysign(M_PI / 2, sinp); // use 90 degrees if out of range
            else
            pitch = asin(sinp);

        // yaw (z-axis rotation)
        double siny = +2.0 * (Q.t() * Q.z() + Q.x() * Q.y());
        double cosy = +1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z());
        yaw = atan2(siny, cosy);
        return WorldLocationAngles(yaw, pitch, roll);
    }


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(gamma, 0.0, "yaw"  );
        visitor.visit(beta , 0.0, "pitch");
        visitor.visit(alpha, 0.0, "roll" );
    }

    WorldLocationAngles toDeg() const {
        return WorldLocationAngles(
                    radToDeg(yaw()),
                    radToDeg(pitch()),
                    radToDeg(roll())
                    );
    }

    WorldLocationAngles toRad() const {
        return WorldLocationAngles(
                    degToRad(yaw()),
                    degToRad(pitch()),
                    degToRad(roll())
                    );
    }

    friend ostream& operator << (ostream &out, WorldLocationAngles &toSave)
    {
        PrinterVisitor printer(out);
        toSave.accept<PrinterVisitor>(printer);
        return out;
    }

    void prettyPrint (ostream &out = cout);

};





/**
 *   Contrary to what Affine3D does this class holds reference frame transformation in camera related terms
 *
 *
 *   Information about camera.
 *   Model is as follows
 *    1. The input world point is X (position)
 *    2. It is then transformed to homogeneous coordinates X'
 *    3. Then Camera Extrinsic parameters matrix is applied
 *       1. Point is moved so that camera becomes at 0
 *           X' = X - Position
 *       1. World is rotated (orientation) to meet the camera position
 *           E = Rotation * X'
 *    4. Then we apply Intrinsic camera transfromation
 *
 *    \f[
 *
 *   \pmatrix { u \cr v \cr t } =
 *
 *   \left( \begin{array}{ccc|c}
 *        \multicolumn{3}{c|}{\multirow{3}{*}{$R_{3 \times 3}$}} &   -T_x \\
 *                 &   &                            &   -T_y \\
 *                 &   &                            &   -T_z
 *   \end{array} \right)
 *   \pmatrix { X \cr Y \cr Z \cr 1 }
 *
 *   \f]
 *   \f[
 *
 *   \pmatrix { x \cr y \cr 1 } =
 *      \pmatrix{
 *       f \over k &       0    & I_x\cr
 *           0     &  f \over k & I_y\cr
 *           0     &       0    & 1  \cr
 *       }
 *       \pmatrix { u \over t \cr v \over t  \cr 1 }
 *
 *    \f]
 *
 **/
class CameraLocationData
{
public:
    Vector3dd position;
    Quaternion orientation;

    explicit CameraLocationData(
            Vector3dd     position = Vector3dd(0.0, 0.0, 1.0),
            Quaternion orientation = Quaternion::Identity()) :
        position(position),
        orientation(orientation)
    {}


    /**
     *  Like Affine3DQ gives us transform to Local Frame to Global Frame,
     *  CameraLocationData acts vica versa from Global Frame to Local Frame
     *
     *  X' = AR * X + AT
     *  X  = AR^{-1} * (X' - AT)
     *
     *
     **/
    explicit CameraLocationData( const Affine3DQ &transform ) :
        position(transform.shift),
        orientation(transform.rotor.conjugated())
    {}

    Affine3DQ toAffine3D() const
    {
        return Affine3DQ(orientation.conjugated(), position);
    }


    /**
     *  This returns Affine3D that acts to the world just as the camera pojects the point
     *  X' = CR * (X - CT)
     *
     *  X' = CR * X  + (- CR * CT)
     *
     *  \attention Generally you don't need to use this function, until you know exactly that you need it
     *
     **/
    Affine3DQ toMockAffine3D() const
    {
        return Affine3DQ(orientation, - (orientation * position));
    }

    /**
     * This returns CameraLocationData that projects the point just as the affine 3d acts to the world
     * Helper function that creates a CameraLocationData that acts just as a Affine3DQ
     *
     * Affine
     *    X' = AR * X + AT
     *
     * Cam
     *    X' = CR * (X - CT) = CR * X - CR * CT
     *
     *    AT = - CR * CT
     *
     *    CR = AR
     *    CT = CR^{-1} (- AT)
     *
     *
     **/
    static CameraLocationData FromMockAffine3D(const Affine3DQ &transform )
    {
        return CameraLocationData(
                transform.rotor.conjugated() * (-transform.shift),
                transform.rotor
        );
    }

    Vector3dd project(const Vector3dd &pt) const
    {
        return orientation * (pt - position);
    }

    Vector3dd worldToCam(const Vector3dd &pt) const
    {
        return project(pt);
    }

    Vector3dd camToWorld(const Vector3dd &pt) const
    {
        return orientation.conjugated() * pt + position;
    }


    /**
     *    If we want to transform the world, let's see how camera model will evolve.
     *
     *    X' =  A * X + b
     *    Camera position is an odinary point, it will change accordingly.
     *    Camera rotation had used to transform Zaxis to the main optical axis.
     *    In a new world Zaxis has shifted to new position, so new camera matrix should first undo this shift,
     *    and then apply the old transform;
     *
     *    R' = R * A^-1
     *
     **/
    void transform(const Quaternion &rotate, const Vector3dd &translate, const double scale = 1.0)
    {
        position    = (rotate * position + translate) * scale;
        orientation = orientation ^ rotate.conjugated();
    }

    void transform(const Affine3DQ &affine, const double scale = 1.0)
    {
        transform(affine.rotor, affine.shift, scale);
    }

    CameraLocationAngles getAngles() const
    {
        return CameraLocationAngles::FromQuaternion(orientation);
    }

    void setAngles(const CameraLocationAngles &angles)
    {
        orientation = angles.toQuaternion();
    }


    /*void transform(const CameraLocationData &outerTransform)
    {
        transform(outerTransform.orientation, outerTransform.position);
    }*/

    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(position,    Vector3dd(0.0, 0.0, -1.0), "position");
        visitor.visit(orientation, Quaternion::Identity()   , "orientation");

        if (visitor.isLoader())
        {
            Quaternion rotation = Quaternion::NaN();
            visitor.visit(rotation, Quaternion::NaN(), "rotation");
            if (!rotation.hasNans()) {
                orientation = rotation.conjugated();
            }
        } else {
            Quaternion rotation = orientation.conjugated();
            visitor.visit(rotation, Quaternion::Identity(), "rotation");
            std::string comment("rotation - is Camera to World and has priority over orientation");
            visitor.visit(comment, std::string()  , "comment");
        }


    }

    /* Pretty print */
    void prettyPrint (ostream &out = cout);
    void prettyPrint1(ostream &out = cout);
};


template<typename DoubleType>
class GenericCameraLocationData
{
public:
    Vector3d<DoubleType> position;
    GenericQuaternion<DoubleType> orientation;


    GenericCameraLocationData(const CameraLocationData &data) :
        position(DoubleType(data.position.x()), DoubleType(data.position.y()), DoubleType(data.position.z())),
        orientation(DoubleType(data.orientation.x()), DoubleType(data.orientation.y()), DoubleType(data.orientation.z()), DoubleType(data.orientation.t()))
    {}

    void transform(const GenericQuaternion<DoubleType> &rotate, const Vector3d<DoubleType> &translate)
    {
        position    = rotate * position + translate;
        orientation = orientation ^ rotate.conjugated();
    }

    Vector3d<DoubleType> project(const Vector3d<DoubleType> &pt) const
    {
        return orientation * (pt - position);
    }

};


} // namespace corecvs

#endif // CALIBRATION_LOCATION_H
