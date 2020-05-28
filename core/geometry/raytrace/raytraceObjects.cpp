#include "geometry/raytrace/raytraceObjects.h"

const double RaytraceableSphere::EPSILON = 0.000001;


/**
   We now can use Sphere3d method for this.
   This method need to be corrected
 **/
bool RaytraceableSphere::intersect(RayIntersection &intersection)
{
    Ray3d ray = intersection.ray;

//    SYNC_PRINT(("RaytraceableSphere::intersect([%lf %lf %lf] -> (%lf %lf %lf))\n", ray.p.x(), ray.p.y(), ray.p.z(), ray.a.x(), ray.a.y(), ray.a.z() ));
//    cout << "RaytraceableSphere::intersect():" << mSphere << endl;


    Vector3dd toCen  = mSphere.c  - ray.p;
    double toCen2 = toCen & toCen;
    double proj  = ray.a & toCen;
    double hdist  = (mSphere.r * mSphere.r) - toCen2 + proj * proj;
    double t2;

    if (hdist < 0) {
        return false;
    }

    hdist = sqrt (hdist);

    if (proj < 0) {
        if (hdist < CORE_ABS(proj) + EPSILON) {
            return false;
        }
    }

    if (hdist > CORE_ABS(proj))
    {
        intersection.t =  hdist + proj;
        t2 =  - hdist + proj;
    }
    else
    {
        intersection.t = proj - hdist;
        t2 = proj + hdist;
    }

    if (CORE_ABS(intersection.t) < EPSILON) intersection.t = t2;

    if (intersection.t > EPSILON) {
        intersection.object = this;
        return true;
    }
    return false;
}

void RaytraceableSphere::normal(RayIntersection &intersection)
{
    intersection.normal = Vector3dd( (intersection.getPoint() - mSphere.c) / mSphere.r);
}

bool RaytraceableSphere::inside(Vector3dd &point)
{
    Vector3dd tmp = mSphere.c - point;
    bool res;
    res = ((tmp & tmp) < (mSphere.r * mSphere.r));
    return res ^ !flag;
}

AxisAlignedBox3d RaytraceableSphere::getBoundingBox()
{
    return AxisAlignedBox3d::ByCenter(mSphere.c, Vector3dd(mSphere.diameter()));
}

bool RaytraceableSphere::toMesh(Mesh3D &target)
{
    target.setColor(RGBColor::FromDouble(color));
    target.addIcoSphere(mSphere, 2);
    return true;
}

bool RaytraceableUnion::intersect(RayIntersection &intersection)
{
    if (opt == NULL)
    {
        RayIntersection best = intersection;
        best.t = std::numeric_limits<double>::max();

        for (Raytraceable *object: elements)
        {
            RayIntersection attempt = intersection;
            if (!object->intersect(attempt)) {
                continue;
            }
            if (attempt.t < best.t)
                best = attempt;
        }

        if (best.t == std::numeric_limits<double>::max()) {
            return false;
        }

        intersection = best;
        return true;
    } else {
        return opt->intersect(intersection);
    }
}

void RaytraceableUnion::normal(RayIntersection & /*intersection*/)
{
    return;
}

bool RaytraceableUnion::inside(Vector3dd &point)
{
    for (Raytraceable *object: elements)
    {
        if (object->inside(point))
            return true;
    }
    return false;
}

bool RaytraceableUnion::toMesh(Mesh3D &target)
{
    for (Raytraceable *object: elements)
    {
        object->toMesh(target);
    }
    return true;
}

bool RaytraceablePlane::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;
    bool hasIntersection = false;
    double t = mPlane.intersectWithP(intersection.ray, &hasIntersection);

    if (!hasIntersection)
        return false;

    if (t > 0.000001) {
        intersection.t = t;
        intersection.object = this;
        return true;
    }
    return false;
}

void RaytraceablePlane::normal(RayIntersection &intersection)
{
    intersection.normal = mPlane.normal();
}

bool RaytraceablePlane::inside(Vector3dd &point)
{
    return mPlane.pointWeight(point);
}

bool RaytraceableTriangle::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;
    double t = 0;

    if (!mTriangle.intersectWithP(intersection.ray, t))
    {
        return false;
    }
    if (t > 0.000001) {
        intersection.t = t;
        intersection.object = this;
        return true;
    }
    return false;
}

void RaytraceableTriangle::normal(RayIntersection &intersection)
{
    intersection.normal = mTriangle.getNormal();
}

bool RaytraceableTriangle::inside(Vector3dd & /*point*/)
{
    return false;
}

RaytraceableMesh::RaytraceableMesh(Mesh3DDecorated *mesh) :
    mMesh(mesh)
{


}

bool RaytraceableMesh::intersect(RayIntersection &intersection)
{

    RayIntersection best = intersection;
    best.t = std::numeric_limits<double>::max();
    // SYNC_PRINT(("RaytraceableMesh::intersect(): entered\n"));

    for (size_t i = 0; i < mMesh->faces.size(); i++)
    {
        Triangle3dd triangle = mMesh->getFaceAsTrinagle(i);

        double t = 0;
        if (!triangle.intersectWithP(intersection.ray, t))
        {
            continue;
        }

        if (t > 0.000001 && t < best.t)
        {
            best.t = t;
            best.normal = triangle.getNormal();
            best.object = this;
        }
    }

    if (best.t == std::numeric_limits<double>::max()) {
        // SYNC_PRINT(("RaytraceableMesh::intersect(): passed\n"));

        return false;
    }

    intersection = best;
    return true;

}

void RaytraceableMesh::normal(RayIntersection & /*intersection*/)
{
    return;
}

bool RaytraceableMesh::inside(Vector3dd & /*point*/)
{
    return false;
}




bool RaytraceableTransform::intersect(RayIntersection &intersection)
{
    RayIntersection trans = intersection;
    trans.ray.transform(mMatrixInv);

    double len = trans.ray.a.l2Metric();
    trans.ray.a /= len;

    //trans.ray.a.normalise();

    if (mObject->intersect(trans)) {
        intersection.object = this;
        intersection.t = trans.t / len;
        intersection.normal = mMatrixInv.t() * trans.normal - mMatrixInv.t() * Vector3dd::Zero();
        return true;
    }
    return false;
}

void RaytraceableTransform::normal(RayIntersection &intersection)
{
    //normal = Vector3dd::OrtZ();
    RayIntersection trans = intersection;    
    trans.ray.transform(mMatrixInv);

    //double scale1 = intersection.ray.a.l2Metric();
    //double scale2 = trans.ray.a.l2Metric();
    //trans.t = trans.t / scale1 * scale2;

    mObject->normal(trans);
    intersection.normal = mMatrixInv.t() * trans.normal - mMatrixInv.t() * Vector3dd::Zero();
    intersection.normal /= intersection.normal.l2Metric();
}

bool RaytraceableTransform::inside(Vector3dd &point)
{
    Vector3dd p = mMatrix * point;
    return mObject->inside(p);
}


void RaytraceableOptiMesh::optimize()
{
    delete_safe(opt);
    opt = new RaytraceableMeshTree();
    for (size_t i = 0; i < mMesh->faces.size(); i++)
    {
        NumTriangle3dd triangle(mMesh->getFaceAsTrinagle(i), (int)i);
        opt->submesh.push_back(triangle);
    }
    opt->subdivide();
    opt->cache();
}

void RaytraceableOptiMesh::dumpToMesh(Mesh3D &mesh, bool plane, bool volume)
{
    if (opt == NULL) {
        SYNC_PRINT(("RaytraceableOptiMesh: not optimized"));
        return;
    }

    opt->dumpToMesh(mesh, 0, plane, volume);

}

bool RaytraceableOptiMesh::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;

    if (opt == NULL) {
        // SYNC_PRINT(("RaytraceableOptiMesh::intersect(): not optimized"));
        return false;
    }

    if (opt->intersect(intersection))
    {
        // SYNC_PRINT(("RaytraceableOptiMesh::intersect(): We have intersection (%lf)\n", intersection.t));
        intersection.object = this;
        return true;
    }
    return false;
}

void RaytraceableOptiMesh::normal(RayIntersection &intersection)
{
//    SYNC_PRINT(("RaytraceableOptiMesh::normal(%d, %d)\n", intersection.payload, mMesh->hasNormals));

    if (intersection.payload != -1 )
    {
        //Vector3d32 face = mMesh->faces[intersection.payload];
        double u = intersection.uvCoord.x();
        double v = intersection.uvCoord.y();

        if(mMesh->hasNormals) {
            Vector3d32 normalId  = mMesh->normalId[intersection.payload];
            Vector3dd n =
                    mMesh->normalCoords[normalId.x()] * (1 - u - v) +
                    mMesh->normalCoords[normalId.y()] * u +
                    mMesh->normalCoords[normalId.z()] * v;
            intersection.normal = n.normalised();
        }

        if (mMesh->hasTexCoords) {
            Vector3d32 textureId = mMesh->texId   [intersection.payload].xyz();
            Vector2dd tex =
                    mMesh->textureCoords[textureId.x()] * (1 - u - v) +
                    mMesh->textureCoords[textureId.y()] * u +
                    mMesh->textureCoords[textureId.z()] * v;
            intersection.texCoord = tex;

        }
//        SYNC_PRINT(("Augmented Normal\n"));
    }
    //intersection.texCoord = mMesh->textureCoords[face.x()];
}

#if 0
bool RaytraceableCylinder::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;

    Plane3d bottom = mCylinder.getBottomPlane();
    Plane3d top    = mCylinder.getTopPlane();

    Ray3d axis = mCylinder.getAxis();




    Ray3d projected = mCylinder.getBottomPlane().projectRayTo(intersection.ray);
    Sphere3d tmp(mCylinder);

    double t[4] = { std::numeric_limits<double>::lowest() };
    bool bc = tmp.intersectWith(projected, t[0], t[1]);

    t[0] /= projected.a.l2Metric();
    t[1] /= projected.a.l2Metric();

    bool b1 = true;
    t[2] = bottom.intersectWithP(intersection.ray, &b1);
    if (!b1) t[2] = std::numeric_limits<double>::lowest();

    bool b2 = true;
    t[3] = top.intersectWithP(intersection.ray, &b2);
    if (!b2) t[3] = std::numeric_limits<double>::lowest();

    double best = std::numeric_limits<double>::max();
    int n = -1;

    for (size_t i = 0; i < CORE_COUNT_OF(t); i++ )
    {
        if (t[i] > 0 && t[i] < best) {
            best = t[i];
            n = i;
        }
    }

    if (n == -1)
    {
        return false;
    }

    intersection.object = this;
    intersection.t = best;

    if (n == 0 || n == 1) {
        intersection.normal = mCylinder.normal;
    } else {
        Vector3dd point = intersection.getPoint();
        intersection.normal = point - axis.projection(point);
        intersection.normal.normalise();
    }
    return true;
}

bool RaytraceableCylinder::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;

    Ray3d ray  = intersection.ray.normalised();
    Ray3d axis = mCylinder.getAxis().normalised();

//    cout << "RaytraceableCylinder::intersect():  ray:" << ray  << endl;
//    cout << "RaytraceableCylinder::intersect(): axis:" << axis << endl;

    Vector3dd coef = ray.intersectCoef(axis);

//    cout << "RaytraceableCylinder::intersect(): coef:" << coef << endl;

    if (fabs(coef.z()) > mCylinder.r)
        return false;

    double ca = (ray.a & axis.a);
    double sa = sqrt(1.0 - (ca * ca));

//    cout << "RaytraceableCylinder::intersect(): ca, sa:" << ca << " " << sa << endl;

    double l1 = coef.z() * ca;
    double l2 = coef.z() * sa;
    double dt = sqrt(mCylinder.r * mCylinder.r - l2 * l2);

    double h1 = coef.y() - l2 * mCylinder.r;
    double h2 = coef.y() + l2 * mCylinder.r;

    if        (coef.x() - dt  > 0.0001 && h1 < mCylinder.height && h1 > 0 ) {
        intersection.t = coef.x() - dt;
        intersection.object = this;
        return true;
    } else if (coef.x() + dt  > 0.0001 && h2 < mCylinder.height && h2 > 0 )  {
        intersection.t = coef.x() + dt;
        intersection.object = this;
        return true;
    }
    return false;
}
#endif


/**
 *  We first transform the ray into cylinder coordinate frame
 *
 *   Z will become axis direction
 *   X and Y will be in the circle plane
 *
 *   cf - is a cylinder frame ray;
 *
 *  We first conpute the intersection in XY frame
 *  pr (plane ray) is a ray XY part - it is shorter then the cf and intersection.ray
 *
 *  After intersection is computed the same paramter t1 and t2 give the point for the input ray
 *
 *
 *
 **/
bool RaytraceableCylinder::intersect(RayIntersection &intersection)
{
    static const double CYL_EPSILON  = 0.00001;
    intersection.object = NULL;

    //Ray3d &ray = intersection.ray;
    Ray3d ray1 = intersection.ray;
    ray1.p -= p;
    Ray3d cf = ray1.transformed(rotation);

    Ray2d    pr   = Ray2d(cf.a.xy(), cf.p.xy());
    Circle2d base = Circle2d(Vector2dd::Zero(), r);

    double t1, t2;
    if (!base.intersectWith(pr, t1, t2))
    {
        return false;
    }


    double len1 = (cf.p.z() + t1 * cf.a.z()) / h;
    double len2 = (cf.p.z() + t2 * cf.a.z()) / h;

#if 0
    double a = !(cylFrame.a.xy());
    double b = cylFrame.a.xy() & cylFrame.p.xy();
    double c = !(cylFrame.p.xy()) - r*r*r*r;
    double d = b * b - a * c;

    if (d <= 0.0) return 0;

    d = sqrt (d);

    double t1 = (-b - d) / a;
    double t2 = (-b + d) / a;


    double len1 = (cylFrame.p.z() + t1 * cylFrame.a.z()) / ray.a.sumAllElementsSq();
    double len2 = (cylFrame.p.z() + t2 * cylFrame.a.z()) / ray.a.sumAllElementsSq();



#endif

    //Vector3dd n = rotation.row(2);



/*    cout << "RaytraceableCylinder::intersect():  ray:" << cylFrame  << endl;
    cout << "RaytraceableCylinder::intersect():    b:" << b << endl;
    cout << "RaytraceableCylinder::intersect():  pos:" << pos << endl;
    cout << "RaytraceableCylinder::intersect():   db:" << db << endl;
    cout << "RaytraceableCylinder::intersect():    t:" << t1 << " " << t2 << endl;

    cout << "RaytraceableCylinder::intersect():  len:" << len1 << " " << len2 << endl;
*/


    //if ( cylFrame.a.z() > CYL_EPSILON || cylFrame.a.z() < -CYL_EPSILON)
    if (1)
    {
        double rsq = r * r;

        Plane3d bottom = Plane3d::FromNormalAndPoint(-Vector3dd::OrtZ(), Vector3dd::Zero()    );
        Plane3d top    = Plane3d::FromNormalAndPoint( Vector3dd::OrtZ(), Vector3dd::OrtZ() * h);

        bool ok1;
        bool ok2;
        double tp1 = bottom.intersectWithP(cf, &ok1);
        double tp2 = top   .intersectWithP(cf, &ok2);
        Vector2dd pp1 = cf.getPoint(tp1).xy();
        Vector2dd pp2 = cf.getPoint(tp2).xy();

        if      (len1 < 0.0)
        {
            if ( !ok1 || (pp1 & pp1) >= rsq ) {
                t1 = -1;
            } else {
                t1 = std::max(tp1, t1);
            }
        }
        else if (len1 > 1.0)
        {
            if ( !ok2 || (pp2 & pp2) >= rsq ) {
                t1 = -1;
            } else {
                t1 = std::max(tp2, t1);
            }
        }

        if      (len2 < 0.0)
        {
            if ( !ok1 || (pp1 & pp1) >= rsq ) {
                t2 = -1;
            } else {
                t2 = std::max(tp1, t2);
            }
        }
        else if ( len2 > 1.0)
        {
            if ( !ok2 || (pp2 & pp2) >= rsq ) {
                t2 = -1;
            } else {
                t2 = std::max(tp2, t2);
            }
        }
    }

    if (t1 > t2) std::swap(t1, t2);
/*

    double a = cylFrame.a.xy().sumAllElementsSq();
    double b = cylFrame.p.xy() & cylFrame.a.xy();
    double c = cylFrame.p.xy().sumAllElementsSq() - r * r * r *r;

    double d = b * b - a * c;

    if (d <= 0.0)
        return false;
    d = sqrt(d);

    double t1 = (-b - d) / a;
    double t2 = (-b + d) / a;

    double len1 = (cylFrame.a.z() + t1*cylFrame.p.z()) / h;
    double len2 = (cylFrame.a.z() + t2*cylFrame.p.z()) / h;
*/

    if ( t1 > CYL_EPSILON)
    {
       intersection.object = this;
       intersection.t      = t1;
       return true;
    }

    if ( t2 > CYL_EPSILON)
    {
       intersection.object = this;
       intersection.t      = t2;
       return true;
    }
    return false;

}

void RaytraceableCylinder::normal(RayIntersection &intersection)
{
    static const double CYL_EPSILON  = 0.00001;
    Vector3dd n = rotation.row(2);
    Vector3dd pos = intersection.getPoint();
    double t = ((pos - p) & n);

    if ( t <  CYL_EPSILON )
    {
        intersection.normal = - n;
        return;
    }
    if ( t > h - CYL_EPSILON) {
        intersection.normal = n;
        return;
    }

    intersection.normal  = (pos - p - n * t).normalised();
    return;
}



bool RaytraceableCylinder::inside(Vector3dd &point)
{
    Vector3dd n = rotation * Vector3dd::OrtZ();
    Ray3d axis(n, p);
    double originProj = axis.projectionP(point);

    if (originProj < 0 || originProj > h)
    {
        return flag;
    }

    double d = axis.distanceTo(point);
    if (d > r)
    {
        return flag;
    }

    return !flag;

}

AxisAlignedBox3d RaytraceableCylinder::getBoundingBox()
{
    AxisAlignedBox3d box(Vector3dd(-r,-r,0), Vector3dd(r,r,h));
    return box.transformedBound(Matrix44(rotation.inv(), p));
}

bool RaytraceableCylinder::toMesh(Mesh3D &target)
{
    target.setColor(RGBColor::FromDouble(color));
    target.mulTransform(Matrix44::Shift(p) * Matrix44(rotation.inv()) * Matrix44::Shift(Vector3dd(0, 0, h / 2.0)));
    target.addCylinder(Vector3dd::Zero(), r, h, 20);
    target.popTransform();
    return true;
}

void RaytraceableUnion::optimize()
{
    PreciseTimer timer = PreciseTimer::CurrentETime();
    delete_safe(opt);
    opt = new RaytraceableSubTree();
    for (size_t i = 0; i < elements.size(); i++)
    {
        opt->submesh.push_back(RaytraceableNodeWrapper(elements[i]));
    }
    opt->subdivide();
    opt->cache();
    SYNC_PRINT(("RaytraceableUnion::optimize(): Optimized %" PRISIZE_T " elements for %2.5f ms \n", elements.size(), timer.usecsToNow() / 1000.0));
}

void RaytraceableCylinder::setPosition(const Affine3DQ &affine)
{
    p = affine.shift;
    rotation = affine.rotor.conjugated().toMatrix();
}

void RaytraceableCylinder::setPosition(const Vector3dd &position)
{
    p = position;
}

void RaytraceableCylinder::setPosition(double x, double y, double z)
{
    setPosition(Vector3dd(x,y,z));
}

void RaytraceableCylinder::setRotation(const Matrix33 &rotation)
{
    this->rotation = rotation.inv();
}

Matrix33 RaytraceableCylinder::getRotationToCylinder()
{
    return rotation;
}
