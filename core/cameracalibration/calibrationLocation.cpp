#include "math/mathUtils.h"
#include "cameracalibration/calibrationLocation.h"
#include "reflection/printerVisitor.h"

namespace corecvs {

using std::endl;

void CameraLocationData::prettyPrint(std::ostream &out)
{
    PrinterVisitor visitor(3, 3, out);
    accept(visitor);
}

void CameraLocationData::prettyPrint1(std::ostream &out)
{
    Quaternion o = orientation.normalised();
    Vector3dd axis = o.getAxis();
    double   angle = radToDeg(o.getAngle());

    out << "Pos:" <<  position << endl;
    out << "Rotation around: " << axis << " angle " << angle << "deg" << endl;
}

void CameraLocationAngles::prettyPrint(std::ostream &out)
{
    out << "Yaw  :" <<  yaw()   << " (" << radToDeg(yaw  ()) << "°)" << endl;
    out << "Pitch:" <<  pitch() << " (" << radToDeg(pitch()) << "°)" << endl;
    out << "Roll :" <<  roll()  << " (" << radToDeg(roll ()) << "°)" << endl;
}

/* World */

void WorldLocationAngles::prettyPrint(std::ostream &out)
{
    out << "World" << endl;
    out << "Yaw  :" <<  yaw()   << " (" << radToDeg(yaw  ()) << "°)" << endl;
    out << "Pitch:" <<  pitch() << " (" << radToDeg(pitch()) << "°)" << endl;
    out << "Roll :" <<  roll()  << " (" << radToDeg(roll ()) << "°)" << endl;
}





} // namespace corecvs
