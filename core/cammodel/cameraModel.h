#ifndef CAMERAMODEL_H
#define CAMERAMODEL_H

#include <vector>
#include <string>
#include <istream>
#include <math.h>   // cbrt
#ifndef cbrt
#define cbrt(x)  pow(x, 1./3)
#endif

#include "core/cammodel/cameraParameters.h"
#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"
#include "core/math/quaternion.h"
#include "core/math/matrix/matrix33.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "core/geometry/line.h"
//#include "core/math/matrix/similarityReconstructor.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/ellipticalApproximation.h"
#include "core/geometry/convexPolyhedron.h"

using std::vector;
using std::istream;

using corecvs::CameraIntrinsicsLegacy;

using corecvs::Vector2dd;
using corecvs::Vector3dd;
using corecvs::Quaternion;
using corecvs::Matrix33;
using corecvs::RGB24Buffer;
using corecvs::ConvexPolyhedron;

/**
 *
 **/
struct NamedPoint
{
    string    name;
    Vector3dd position;
    Vector3dd sqerror;

    NamedPoint() :
        position(0.0),
        sqerror(0.0)
    {}

    void transform(const Matrix33 &matrix, const Vector3dd &translate)
    {
        position = matrix * position + translate;
    }
};

/**
 *   Information about camera.
 *   Model is as follows
 *    1. The input world point is X
 *    2. It is then transformed to homogeneous coordinates X'
 *    3. Then Camera Extrinsic parameters matrix is applied
 *       1. Point is moved so that camera becomes at 0
 *           X' = X - Position
 *       1. World is rotated to meet the camera position
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
class BaseCameraModel : public NamedPoint
{

};

class CameraModelLegacy : public BaseCameraModel
{
public:
    enum LockedMask {
        LOCK_FOCAL      = 1,
        LOCK_POSITION   = 2,
        LOCK_ROTATION   = 4,
        LOCK_DISTORTION = 8,
        LOCK_NONE       = 0,
        LOCK_ALL        = 15
    };

    /* Basic parameters */
    Matrix33 rotation;

    /**/
    std::string filename;
    std::string filepath;
    double      focal;
    Vector2dd   optCenter;
    Vector2dd   resolution;

    // Vector3dd origin;       /**< R * position */
    // Vector3dd axisAngles;
    // Quaternion quaternion;
    double      distortion;
    Vector3dd   geographic;

    //
    LockedMask locked;

    CameraModelLegacy();

    bool checkAsserts(void);
    void finalizeLoad(void);

    void print(void);

    /**
    *  derived parameters
    **/
    CameraIntrinsicsLegacy cameraIntrinsics;

    Ray3d rayFromPixel(const Vector2dd &point);
    Ray3d rayFromCenter();

    Vector3dd forwardDirection() const;
    Vector3dd topDirection() const;
    Vector3dd rightDirection() const;

    Matrix44  getCameraMatrix() const;
    Vector3dd getCameraTVector() const;

    ConvexPolyhedron getViewport(const Vector2dd &p1, const Vector2dd &p2);

    /**
     *    If we want to transform the world, let's see how camera model will evolve.
     *
     *    X' =  A * X + b
     *    Camera position is an odinary point, it will change accordingly.
     *    Camera rotation had used to transform Zaxis to the main optical axis.
     *    In a new world Zasix has shifted to new position, so new camera matrix should first undo this shift,
     *    and then apply the old transform;
     *
     *    R' = R * A^-1
     *
     **/
    void transform(const Matrix33 &matrix, const Vector3dd &translate)
    {
        Matrix33 unscale = matrix / cbrt(matrix.det());
        NamedPoint::transform(matrix, translate);
        //rotation =  rotation * matrix;
        rotation = rotation * unscale.inv();
        //quaternion = rotation
    }
};

namespace ReprojectPath {
enum ReprojectPath{
    FEATURE_PATH,
    INPUT_PATH
};
}

class CameraLegacy : public CameraModelLegacy
{
public:
    RGB24Buffer *image;
    RGB24Buffer *work;

    EllipticalApproximation featuresStats;
    EllipticalApproximation inputStats;

    CameraLegacy() :
        image(NULL),
        work(NULL)
    {}

    void resetWork() {
        delete_safe (work);
        if (image != NULL) {
            work = new RGB24Buffer(image);
        }
    }

     void resetStats() {
         featuresStats.reset();
         inputStats.reset();
     }

     void dropImages() {
         delete_safe (work);
         delete_safe (image);
     }

     EllipticalApproximation &getStats(ReprojectPath::ReprojectPath path);
};


#endif // CAMERAMODEL_H
