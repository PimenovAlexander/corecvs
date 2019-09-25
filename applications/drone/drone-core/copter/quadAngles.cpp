#include "quadAngles.h"

using namespace std;
using namespace corecvs;

void QuadAngles::prettyPrint(std::ostream &out)
{
    out << "Yaw  :" <<  yaw()   << " (" << radToDeg(yaw  ()) << "°)" << endl;
    out << "Pitch:" <<  pitch() << " (" << radToDeg(pitch()) << "°)" << endl;
    out << "Roll :" <<  roll()  << " (" << radToDeg(roll ()) << "°)" << endl;
}

corecvs::Matrix33 QuadAngles::toMatrix() const
{
    return
            Matrix33::RotationZ(roll()) *
            Matrix33::RotationY(yaw()) *
            Matrix33::RotationX(pitch());
}

Quaternion QuadAngles::toQuaternion() const
{
    return
            Quaternion::RotationZ(roll()) ^
            Quaternion::RotationY(yaw()) ^
            Quaternion::RotationX(pitch());
}

ostream &operator <<(ostream &out, QuadAngles &toSave)
{
    corecvs::PrinterVisitor printer(out);
    toSave.accept<corecvs::PrinterVisitor>(printer);
    return out;
}
