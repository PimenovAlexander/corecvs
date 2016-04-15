#include <fstream>
#include <iostream>
#include <cstddef>
#include <string>

#include "cameraModel.h"

#include "log.h"
#include "vector2d.h"
#include "vector3d.h"
#include "quaternion.h"
#include "matrix33.h"
#include "line.h"

using std::ifstream;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::Quaternion;
using corecvs::Matrix33;
using corecvs::Plane3d;


CameraModelLegacy::CameraModelLegacy() :
    rotation(1.0),

    focal(1.0),
    optCenter(0.5, 0.5),
    resolution(1.0, 1.0),
    //origin(0.0),
    //axisAngles(0.0),
    //quaternion(Quaternion::RotationIdentity()),
    distortion(0.0),
    geographic(0.0),
    locked(LOCK_NONE)
{}

bool CameraModelLegacy::checkAsserts()
{
//    SYNC_PRINT(("BundlerCamera::checkAsserts(): called\n"));
   /* corecvs::Quaternion fromMatrix = corecvs::Quaternion::FromMatrix(rotation);
    if ( (!(quaternion - fromMatrix))  > 0.00001 )
    {
        SYNC_PRINT(("BundlerCamera::checkAsserts(): assert failed\n"));
        cout << "quaternion orig :" << quaternion << endl;
        cout << "quaternion mat  :" << fromMatrix << endl;
        return false;
    }*/

    /*Vector3dd t  = position;
    Vector3dd t1 = -(rotation * origin);

    if ( (!(t1 - t))  > 0.00001 )
    {
        SYNC_PRINT(("BundlerCamera::checkAsserts(): assert failed\n"));
        cout << "translation orig:" << t << endl;
        cout << "translation mat :" << t1 << endl;
        return false;
    }*/

//    SYNC_PRINT(("BundlerCamera::checkAsserts(): exited\n"));
    return true;
}

void CameraModelLegacy::finalizeLoad(void)
{
    cameraIntrinsics = CameraIntrinsicsLegacy(resolution, optCenter, focal, 1.0);
}

void CameraModelLegacy::print()
{
    L_INFO << "Name:       " << filename;
    L_INFO << "Full name:  " << filepath;
    L_INFO << "Focal len:  " << focal;
    L_INFO << "Opt center: " << optCenter;
    L_INFO << "Pos:        " << position;
  //L_INFO << "Origin:     " << origin;
  //L_INFO << "Axis angles:" << axisAngles;
  //L_INFO << "Q Rotation: " << quaternion;
    L_INFO << "Rotation:   \n" << rotation;
    L_INFO << "Distortion: " << distortion;
    L_INFO << "Coor:       " << geographic;
}

Ray3d CameraModelLegacy::rayFromPixel(const Vector2dd &point)
{
 //   Matrix44 Kinv = cameraIntrinsics.getInvKMatrix();
 //  Vector3dd direction = Kinv * Vector3dd(point.x(), point.y(), 1.0);

    Vector3dd direction((point.x() - optCenter.x()) / focal, (point.y() - optCenter.y()) / focal, 1.0);
    Ray3d ray(rotation.inv() * direction , position);
    return ray;
}

Ray3d CameraModelLegacy::rayFromCenter()
{
    return rayFromPixel(optCenter);
}

Vector3dd CameraModelLegacy::forwardDirection() const
{
    //return rotation.column(2);
    return rotation.transposed() * Vector3dd(0.0, 0.0, 1.0);
}

Vector3dd CameraModelLegacy::topDirection() const
{
    return rotation.transposed() * Vector3dd(0.0, 1.0, 0.0);
}

Vector3dd CameraModelLegacy::rightDirection() const
{
    return rotation.transposed() * Vector3dd(1.0, 0.0, 0.0);
}


Matrix44 CameraModelLegacy::getCameraMatrix() const
{
    return cameraIntrinsics.getKMatrix() * rotation * Matrix44::Shift(-position);
}

Vector3dd CameraModelLegacy::getCameraTVector() const
{
    return rotation * (-position);
}

ConvexPolyhedron CameraModelLegacy::getViewport(const Vector2dd &p1, const Vector2dd &p3)
{
    ConvexPolyhedron toReturn;
    Matrix33 invrotation = rotation.inv();

    //Vector3dd direction1((point.x() - optCenter.x()) / focal, (point.y() - optCenter.y()) / focal, 1.0);

    Vector2dd p2 = Vector2dd(p3.x(), p1.y());
    Vector2dd p4 = Vector2dd(p1.x(), p3.y());

    Vector3dd d1 = position + invrotation * Vector3dd((p1 - optCenter) / focal, 1.0);
    Vector3dd d2 = position + invrotation * Vector3dd((p2 - optCenter) / focal, 1.0);
    Vector3dd d3 = position + invrotation * Vector3dd((p3 - optCenter) / focal, 1.0);
    Vector3dd d4 = position + invrotation * Vector3dd((p4 - optCenter) / focal, 1.0);


    toReturn.faces.push_back( Plane3d::FromPoints(position, d1, d2) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d2, d3) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d3, d4) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d4, d1) );

    return toReturn;
}


EllipticalApproximation &CameraLegacy::getStats(ReprojectPath::ReprojectPath path)
{
    if (path == ReprojectPath::FEATURE_PATH)
        return featuresStats;
    else
        return inputStats;
}
