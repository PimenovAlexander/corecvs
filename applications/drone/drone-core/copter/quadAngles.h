#ifndef QUADANGLES_H
#define QUADANGLES_H

#include <iostream>
#include "core/utils/global.h"
#include "core/math/mathUtils.h"

#include "core/math/eulerAngles.h"

#include <core/reflection/printerVisitor.h>



class QuadAngles : public corecvs::EulerAngles
{
public:
    QuadAngles();


    QuadAngles(double yaw = 0.0, double pitch = 0.0, double roll = 0.0) :
        EulerAngles(yaw, pitch, roll)
    {}

    static QuadAngles FromAnglesDeg(double yawDeg, double pitchDeg, double rollDeg)
    {
        return QuadAngles(
                    corecvs::degToRad(yawDeg),
                    corecvs::degToRad(pitchDeg),
                    corecvs::degToRad(rollDeg));
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

    corecvs::Matrix33 toMatrix() const;
    corecvs::Quaternion toQuaternion() const;

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
    static QuadAngles FromQuaternion(const corecvs::Quaternion &Q)
    {
        double yaw   = asin  (2.0 * (Q.t() * Q.y() - Q.z() * Q.x()));
        double pitch = atan2 (2.0 * (Q.t() * Q.x() + Q.y() * Q.z()),1.0 - 2.0 * (Q.x() * Q.x() + Q.y() * Q.y()));
        double roll  = atan2 (2.0 * (Q.t() * Q.z() + Q.x() * Q.y()),1.0 - 2.0 * (Q.y() * Q.y() + Q.z() * Q.z()));
        return QuadAngles(yaw, pitch, roll);
    }


    template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(alpha, 0.0, "yaw"  );
        visitor.visit(beta , 0.0, "pitch");
        visitor.visit(gamma, 0.0, "roll" );
    }

    QuadAngles toDeg() const {
        return QuadAngles(
                    corecvs::radToDeg(yaw()),
                    corecvs::radToDeg(pitch()),
                    corecvs::radToDeg(roll())
                    );
    }

    QuadAngles toRad() const {
        return QuadAngles(
                    corecvs::degToRad(yaw()),
                    corecvs::degToRad(pitch()),
                    corecvs::degToRad(roll())
                    );
    }

    friend std::ostream& operator << (std::ostream &out, QuadAngles &toSave);

    void prettyPrint (std::ostream &out = std::cout);


};

#endif // QUADANGLES_H
