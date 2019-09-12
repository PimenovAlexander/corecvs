#include <fstream>

#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/convexHull.h"

namespace corecvs {

Matrix33 CameraModel::Fundamental(const Matrix44 &L, const Matrix44 &R)
{
    const Matrix44 P[2] = {R, L};
    Matrix44 t[3][3];
    int rows[3][3][2][3]
// C++ generally and corecvs particullary are not very cool at clever indexing
#define X1 {0, 1, 2}
#define X2 {0, 2, 0}
#define X3 {0, 0, 1}
#define Y1 {1, 1, 2}
#define Y2 {1, 2, 0}
#define Y3 {1, 0, 1}
     = {
        {{ X1, Y1 }, { X2, Y1 }, { X3, Y1 }},
        {{ X1, Y2 }, { X2, Y2 }, { X3, Y2 }},
        {{ X1, Y3 }, { X2, Y3 }, { X3, Y3 }}
     };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < 2; ++k)
                for (int l = 0; l < 4; ++l)
                {
                    t[i][j].a(k * 2,     l) = P[rows[i][j][k][0]].a(rows[i][j][k][1], l);
                    t[i][j].a(k * 2 + 1, l) = P[rows[i][j][k][0]].a(rows[i][j][k][2], l);
                }
    Matrix33 F;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            F.a(i, j) = t[i][j].det();
    return F;
}

Matrix33 CameraModel::fundamentalTo(const CameraModel &right) const
{
    if (!intrinsics->isPinhole() || !right.intrinsics->isPinhole())
    {
        return EssentialMatrix();
    }

    PinholeCameraIntrinsics *pinhole1 = static_cast<PinholeCameraIntrinsics *>(intrinsics.get());
    PinholeCameraIntrinsics *pinhole2 = static_cast<PinholeCameraIntrinsics *>(right.intrinsics.get());


    Matrix33 K1 = pinhole1->getKMatrix33();
    Matrix33 K2 = pinhole2->getKMatrix33();
    return K1.inv().transposed() * essentialTo(right) * K2.inv();
}
Matrix33 CameraModel::essentialTo  (const CameraModel &right) const
{
    return (Matrix33)essentialDecomposition(right);
}

EssentialDecomposition CameraModel::essentialDecomposition(const CameraModel &right) const
{
    return CameraModel::ComputeEssentialDecomposition(extrinsics, right.extrinsics);
}

EssentialDecomposition CameraModel::ComputeEssentialDecomposition(const CameraLocationData &thisData, const CameraLocationData &otherData)
{
    Quaternion R =  thisData.orientation ^ otherData.orientation.conjugated();
    Vector3dd  T =  thisData.orientation * (-thisData.position + otherData.position);
    return EssentialDecomposition(R.toMatrix(), T);
}

PlaneFrame CameraModel::getVirtualScreen(double distance) const
{
    if (!intrinsics->isPinhole())
    {
        return PlaneFrame();
    }
    PinholeCameraIntrinsics *pinhole = static_cast<PinholeCameraIntrinsics *>(intrinsics.get());


    Vector3dd topLeft     = Vector3dd(              0,             0,  1) * distance;
    Vector3dd topRight    = Vector3dd( pinhole->w()  ,             0,  1) * distance;
    Vector3dd bottomLeft  = Vector3dd(              0,  pinhole->h(),  1) * distance;

    PlaneFrame toReturn;

    Matrix33 invK = pinhole->getInvKMatrix33();
    toReturn.p1  = extrinsics.camToWorld(invK * topLeft);
    Vector3dd d1 = extrinsics.camToWorld(invK * topRight  ) - toReturn.p1;
    Vector3dd d2 = extrinsics.camToWorld(invK * bottomLeft) - toReturn.p1;

    toReturn.e1 = (d1 / pinhole->w() );
    toReturn.e2 = (d2 / pinhole->h() );

    return toReturn;
}

Polygon removeDuplicateVertices(const Polygon& polygon)
{
    Polygon filteredPolygon;
    for (auto& vertex : polygon)
    {
        if (filteredPolygon.end() == std::find_if(filteredPolygon.begin(), filteredPolygon.end(), [&] ( const corecvs::Vector2dd& p ) -> bool { return (p - vertex).l2Metric() < 1.e-9; }))
            filteredPolygon.push_back(vertex);
    }

    return filteredPolygon;
}

Polygon CameraModel::projectViewport(const CameraModel &right, double pyramidLength1, double pyramidLength2) const
{
    static bool trace = false;

    ConvexPolyhedron viewport0 =       getCameraViewport(pyramidLength1);
    ConvexPolyhedron viewport1 = right.getCameraViewport(pyramidLength2);

    if (trace) {
        cout << "viewport0:" << viewport0 << endl;
        cout << "viewport1:" << viewport1 << endl;
    }

    /* We are inside other viewport. evey pixel is a possible projection */
    if (viewport1.isInside(extrinsics.toAffine3D().shift))
    {
        if (trace) {
            cout << "One viewport starts inside another" << endl;
        }
        return getCameraViewportPolygon();
    }

    const int SIDE_STEPS = 30;
    std::vector<Ray3d> rays1;
    rays1.reserve(SIDE_STEPS * 4);
    {
        Polygon basePoints = right.getCameraViewportPolygon();

        for (size_t point = 0; point < basePoints.size(); point++)
        {
            for (int i = 0; i < SIDE_STEPS; i++)
            {
                Vector2dd p1 = basePoints.getPoint    ((int)point);
                Vector2dd p2 = basePoints.getNextPoint((int)point);
                Vector2dd p = lerp(p1, p2, i, 0.0, SIDE_STEPS);
                Ray3d r  = right.rayFromPixel(p);
                if (pyramidLength2 > 0) {
                    r.a *= pyramidLength2;
                }
                rays1.push_back(r);
            }
        }
    }

    if (trace) {
        cout << "Rays: ";
        for (Ray3d &ray : rays1) {
              cout << ray << endl;
        }
    }

    /* ==== */
    Matrix44 T = getCameraMatrix();

    /* We go with ray analysis instead of essential matrix beacause it possibly gives
     * more semanticly valuable info
     */

    vector<Vector2dd> points;
    for (size_t rayId = 0; rayId < rays1.size(); rayId++ )
    {
        Ray3d &ray = rays1[rayId];
        double t1 = 0;
        double t2 = 0;
        bool hasIntersection = viewport0.intersectWith(ray, t1, t2);

        if (trace) {
            cout << "Ray " << rayId << " i:" << (hasIntersection ? "true" :  "false") << " t1:" << t1 << " t2:" << t2 << endl;
        }

        if (hasIntersection && t2 > 0.0)
        {
            if (t1 < 0.0) t1 = 0.0;
            if (t2 > 1.0 && pyramidLength2 > 0) t2 = 1.0;

            FixedVector<double, 4> out1 = (T * ray.getProjectivePoint(t1));
            FixedVector<double, 4> out2 = (T * ray.getProjectivePoint(t2));
            Vector2dd pos1 = Vector2dd(out1[0], out1[1]) / out1[2];
            Vector2dd pos2 = Vector2dd(out2[0], out2[1]) / out2[2];
            points.push_back(pos1);
            points.push_back(pos2);
        }
    }

    if (trace) {
        cout << "Points: ";
        for (Vector2dd &point : points) {
            cout << point << endl;
        }
    }
    // return removeDuplicateVertices(ConvexHull::GrahamScan(points)); // No need for this now
    return ConvexHull::GrahamScan(points);
}



/* Camera */

/**
 * Returns a ray in a world coordinate system that originates at the camera position and goes through
 * given pixel. This method ignores distortion.
 *
 *  \param point - a point in image coorinates
 *
 **/
Ray3d CameraModel::rayFromPixel(const Vector2dd &point) const
{
    Vector3dd direction = intrinsics->reverse(point);
    Ray3d ray(extrinsics.orientation.conjugated() * direction, extrinsics.position);
    return ray;
}

Ray3d CameraModel::rayFromCenter()
{
    return rayFromPixel(intrinsics->principal());
}

Vector3dd CameraModel::forwardDirection() const
{
    //return rotation.column(2);
    return extrinsics.orientation.conjugated() * Vector3dd(0.0, 0.0, 1.0);
}

Vector3dd CameraModel::topDirection() const
{
    return extrinsics.orientation.conjugated() * Vector3dd(0.0, 1.0, 0.0);
}

Vector3dd CameraModel::rightDirection() const
{
    return extrinsics.orientation.conjugated() * Vector3dd(1.0, 0.0, 0.0);
}

Matrix33 CameraModel::getRotationMatrix() const
{
    return extrinsics.orientation.toMatrix();
}

Matrix44 CameraModel::getCameraMatrix() const
{
    PinholeCameraIntrinsics *p = getPinhole();

    if (p != NULL) {
        return p->getKMatrix() * Matrix44(getRotationMatrix()) * Matrix44::Shift(-extrinsics.position);
    }
    return Matrix44(getRotationMatrix()) * Matrix44::Shift(-extrinsics.position);
}

Matrix44 CameraModel::getPositionMatrix() const
{
    return Matrix44(getRotationMatrix()) * Matrix44::Shift(-extrinsics.position);
}

Vector3dd CameraModel::getCameraTVector() const
{
    return extrinsics.orientation * (-extrinsics.position);
}

ConvexPolyhedron CameraModel::getViewport(const Vector2dd &p1, const Vector2dd &p3) const
{
    ConvexPolyhedron toReturn;
    Vector3dd position = extrinsics.position;

    //Vector3dd direction1((point.x() - optCenter.x()) / focal, (point.y() - optCenter.y()) / focal, 1.0);

    Vector2dd p2 = Vector2dd(p3.x(), p1.y());
    Vector2dd p4 = Vector2dd(p1.x(), p3.y());

    Vector3dd d1 = extrinsics.camToWorld(intrinsics->reverse(p1));
    Vector3dd d2 = extrinsics.camToWorld(intrinsics->reverse(p2));
    Vector3dd d3 = extrinsics.camToWorld(intrinsics->reverse(p3));
    Vector3dd d4 = extrinsics.camToWorld(intrinsics->reverse(p4));

    toReturn.faces.push_back( Plane3d::FromPoints(position, d1, d2) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d2, d3) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d3, d4) );
    toReturn.faces.push_back( Plane3d::FromPoints(position, d4, d1) );

    return toReturn;
}



ConvexPolyhedron  CameraModel::getCameraViewport(double farPlane) const
{


    ConvexPolyhedron p = getViewport( Vector2dd::Zero(), intrinsics->size() );
    if ( farPlane > 0 )
    {
        Vector3dd position = extrinsics.position;
        Vector3dd center = extrinsics.camToWorld( intrinsics->reverse( intrinsics->principal() ));
        p.faces.push_back( Plane3d::FromNormalAndPoint( -center, position + center.normalised() * farPlane));
     }
    return p;
}

vector<GenericTriangle<Vector4dd> > CameraModel::getCameraViewportSides() const
{
    vector<GenericTriangle<Vector4dd> > sides;

    Vector2dd p1 = Vector2dd::Zero();
    Vector2dd p3 = intrinsics->size();
    Vector2dd p2 = Vector2dd(p3.x(), p1.y());
    Vector2dd p4 = Vector2dd(p1.x(), p3.y());

    Vector4dd position(extrinsics.position, 1.0);
    Vector4dd d1(extrinsics.camToWorld(intrinsics->reverse(p1)), 0.0);
    Vector4dd d2(extrinsics.camToWorld(intrinsics->reverse(p2)), 0.0);
    Vector4dd d3(extrinsics.camToWorld(intrinsics->reverse(p3)), 0.0);
    Vector4dd d4(extrinsics.camToWorld(intrinsics->reverse(p4)), 0.0);

    sides.push_back(GenericTriangle<Vector4dd>(position, d1, d2));
    sides.push_back(GenericTriangle<Vector4dd>(position, d2, d3));
    sides.push_back(GenericTriangle<Vector4dd>(position, d3, d4));
    sides.push_back(GenericTriangle<Vector4dd>(position, d4, d1));
    return sides;

}

vector<Vector4dd> CameraModel::getCameraViewportPyramid() const
{
    vector<Vector4dd> pyramid;

    Vector2dd p1 = Vector2dd::Zero();
    Vector2dd p3 = intrinsics->size();
    Vector2dd p2 = Vector2dd(p3.x(), p1.y());
    Vector2dd p4 = Vector2dd(p1.x(), p3.y());

    Vector4dd position(extrinsics.position, 1.0);
    Vector4dd d1(extrinsics.camToWorld(intrinsics->reverse(p1)), 0.0);
    Vector4dd d2(extrinsics.camToWorld(intrinsics->reverse(p2)), 0.0);
    Vector4dd d3(extrinsics.camToWorld(intrinsics->reverse(p3)), 0.0);
    Vector4dd d4(extrinsics.camToWorld(intrinsics->reverse(p4)), 0.0);

    pyramid.push_back(position);
    pyramid.push_back(d1);
    pyramid.push_back(d2);
    pyramid.push_back(d3);
    pyramid.push_back(d4);

    return pyramid;
}

Polygon CameraModel::getCameraViewportPolygon() const
{
    Polygon polygon = {
        Vector2dd::Zero(),
        Vector2dd(intrinsics->w(), 0),
        intrinsics->size(),
        Vector2dd(0, intrinsics->h()),
    };
    return polygon;
}



#ifndef Rect
typedef std::array<corecvs::Vector2dd, 2> Rect;
#endif

void corecvs::CameraModel::estimateUndistortedSize(const DistortionApplicationParameters &applicationParams)
{
    Rect input = { corecvs::Vector2dd(0.0, 0.0), intrinsics->distortedSize() }, output;
    Rect outCir, outIns;

    distortion.getCircumscribedImageRect(input[0], input[1], outCir[0], outCir[1]);
    distortion.getInscribedImageRect    (input[0], input[1], outIns[0], outIns[1]);

    output = input;
    corecvs::Vector2dd shift(0.0);

    switch (applicationParams.resizePolicy())
    {
        case DistortionResizePolicy::FORCE_SIZE:
            output = { corecvs::Vector2dd(0.0, 0.0), intrinsics->size() };
            shift[0] = distortion.principalX() / intrinsics->distortedSize().x() * output[1][0];
            shift[1] = distortion.principalY() / intrinsics->distortedSize().y() * output[1][1];
            break;
        case DistortionResizePolicy::TO_FIT_RESULT:
            output = outCir;
            shift = -output[0];
            output[1] -= output[0];
            output[0] -= output[0];
            break;
        case DistortionResizePolicy::TO_NO_GAPS:
            output = outIns;
            shift = -output[0];
            output[1] -= output[0];
            output[0] -= output[0];
            break;
        case DistortionResizePolicy::NO_CHANGE:
        default:
            break;
    }

    double w = intrinsics->distortedSize().x();
    double h = intrinsics->distortedSize().y();

    intrinsics->size() = output[1] - output[0];
    double newW = intrinsics->size().x();
    double newH = intrinsics->size().y();

    if (applicationParams.adoptScale())
    {
        double aspect = std::max(newW / w, newH / h);
        distortion.setScale(1.0 / aspect);
    }
    else
    {
        /*
         * In order to maintain idempotence of undistorted size estimation
         * one needs to update intrinsics.principal only by shift,
         * because [in the next call with the same DistortionApplicationParameters]
         * shift should be ~0, since it will be eliminated by distortion.mShift*
         */
        // MEFIXASAP

        //intrinsics.principal += shift;
        distortion.mShiftX += shift[0];
        distortion.mShiftY += shift[1];
    }
}


void CameraModel::prettyPrint(std::ostream &out)
{
    PrinterVisitor visitor(3, 3, out);
    cout << "Camera:" << nameId << std::endl;
    accept(visitor);
}





} // namespace corecvs

