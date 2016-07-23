#include "raytraceObjects.h"

const double RaytraceableSphere::EPSILON = 0.000001;

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

void RaytraceableSphere::normal(const Vector3dd &vector, Vector3dd &normal)
{
    normal = Vector3dd( (vector - mSphere.c) / mSphere.r);
}

bool RaytraceableSphere::inside(Vector3dd &point)
{
    Vector3dd tmp = mSphere.c - point;
    bool res;
    res = ((tmp & tmp) < (mSphere.r * mSphere.r));
    return res ^ !flag;
}

bool RaytraceableUnion::intersect(RayIntersection &intersection)
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
}

void RaytraceableUnion::normal(const Vector3dd &vector, Vector3dd &normal)
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

void RaytraceablePlane::normal(const Vector3dd &vector, Vector3dd &normal)
{
    normal = mPlane.normal();
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

void RaytraceableTriangle::normal(const Vector3dd &vector, Vector3dd &normal)
{
    normal = mTriangle.getNormal();
}

bool RaytraceableTriangle::inside(Vector3dd &point)
{
    return false;
}

RaytraceableMesh::RaytraceableMesh(Mesh3D *mesh) :
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

void RaytraceableMesh::normal(const Vector3dd &vector, Vector3dd &normal)
{
    return;
}

bool RaytraceableMesh::inside(Vector3dd &point)
{
    return false;
}




bool RaytraceableTransform::intersect(RayIntersection &intersection)
{
    RayIntersection trans = intersection;
    trans.ray.p = mMatrixInv * intersection.ray.p;
    trans.ray.a = mMatrixInv * intersection.ray.a;
    double len = trans.ray.a.l2Metric();
    trans.ray.a /= len;

    //trans.ray.a.normalise();

    if (mObject->intersect(trans)) {
        intersection.object = this;
        intersection.t = trans.t / len;
        //intersection.normal = mMatrix.inverted() * trans.normal;
        return true;
    }
    return false;
}

void RaytraceableTransform::normal(const Vector3dd &vector, Vector3dd &normal)
{
    //normal = Vector3dd::OrtZ();
    Vector3dd p = mMatrixInv * vector;
    mObject->normal(p, normal);
    normal = mMatrix * normal;
    normal.normalise();
}

bool RaytraceableTransform::inside(Vector3dd &point)
{
    Vector3dd p = mMatrix * point;
    return mObject->inside(p);
}

bool RaytraceableOptiMesh::TreeNode::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;
    double t = 0;

    double d1,d2;
    if (!box.intersectWith(intersection.ray, d1, d2))
        return false;

    if (d2 <= 0)
        return false;
    /*if (!box.intersectWith(intersection.ray, d1, d2))
        return false;*/

    RayIntersection best = intersection;
    best.t = std::numeric_limits<double>::max();

    /*for (Triangle3dd &triangle : middle)
    {
        if (!triangle.intersectWithP(intersection.ray, t))
            continue;

        if (t > 0.000001 && t < best.t) {
            best.t = t;
            best.normal = triangle.getNormal();
        }
    }*/

    for (PlaneFrame &triangle : cached)
    {
        double u, v;
        if (!triangle.intersectWithP(intersection.ray, t, u, v))
            continue;

        if (t > 0.000001 && t < best.t) {
            best.t = t;
            best.normal = triangle.getNormal();
            best.texCoord = Vector2dd(u, v);
        }
    }

    if (middle != NULL) {
        bool result = middle->intersect(intersection);
        if (result) {
            if (intersection.t > 0.000001 && intersection.t < best.t) {
                best = intersection;
            }
        }
    }

    bool side = plane.pointWeight(intersection.ray.p) > 0;
    TreeNode *close = side ? left : right;
    TreeNode *far   = side ? right : left;


    if (close != NULL) {
        bool result = close->intersect(intersection);
        if (result) {
            if (intersection.t > 0.000001 && intersection.t < best.t) {
                best = intersection;
            }
        }
    }


    if (far != NULL) {
        bool result = far->intersect(intersection);
        if (result) {
            if (intersection.t > 0.000001 && intersection.t < best.t) {
                best = intersection;
            }
        }
    }

    if (best.t != std::numeric_limits<double>::max()) {
        intersection = best;
        return true;
    }
    return false;
}

void RaytraceableOptiMesh::TreeNode::subdivide()
{
    //SYNC_PRINT(("RaytraceableOptiMesh::TreeNode::subdivide() : %u nodes", middle.size()));
    if (submesh.size() == 0)
    {
        SYNC_PRINT(("RaytraceableOptiMesh::TreeNode::subdivide() : empty node\n"));
        return;
    }

    Vector3dd minP = Vector3dd(numeric_limits<double>::max());
    Vector3dd maxP = Vector3dd(numeric_limits<double>::lowest());

    EllipticalApproximation3d approx;
    for (const Triangle3dd &triangle : submesh)
    {
        approx.addPoint(triangle.p1());
        approx.addPoint(triangle.p2());
        approx.addPoint(triangle.p3());

        for (int i = 0; i < Triangle3dd::SIZE; i++)
        {
            for (int j = 0; j < Vector3dd::LENGTH; j++)
            {
                if (minP[j] > triangle.p[i][j]) minP[j] = triangle.p[i][j];
                if (maxP[j] < triangle.p[i][j]) maxP[j] = triangle.p[i][j];
            }
        }
    }

    Vector3dd center = approx.getCenter();
    approx.getEllipseParameters();

    Vector3dd normal = approx.mAxes[0];
    plane = Plane3d::FromNormalAndPoint(normal, center);

    double radius = 0;
    for (const Triangle3dd &triangle : submesh)
    {
        for (int p = 0; p < 3; p++)
        {
            double d = (triangle.p[p] - center).l2Metric();
            if (radius < d)
                radius = d;
        }
    }
    bound = Sphere3d(center, radius + 0.000001);
    box = AxisAlignedBox3d(minP, maxP);

    if (submesh.size() <= 3)
        return;

    vector<Triangle3dd> m;
    vector<Triangle3dd> l;
    vector<Triangle3dd> r;
    for (const Triangle3dd &triangle : submesh)
    {
        bool b1 = (plane.pointWeight(triangle.p1()) > 0);
        bool b2 = (plane.pointWeight(triangle.p2()) > 0);
        bool b3 = (plane.pointWeight(triangle.p3()) > 0);

        if (b1 && b2 && b3) {
            l.push_back(triangle);
            continue;
        }

        if (!b1 && !b2 && !b3) {
            r.push_back(triangle);
            continue;
        }

        m.push_back(triangle);
    }

    /* Check if there was a subdivison actually */
    if (m.size() == submesh.size())
        return;
    if (l.size() == submesh.size())
        return;
    if (r.size() == submesh.size())
        return;


    //submesh = m;
    delete_safe(middle);
    delete_safe(left);
    delete_safe(right);

    //SYNC_PRINT(("RaytraceableOptiMesh::TreeNode::subdivide() : groups %u (%u | %u) nodes\n", m.size(), l.size(), r.size()));

    if (!m.empty()) {
        middle = new TreeNode;
        middle->submesh = m;
        middle->subdivide();
    }
    submesh.clear();

    if (!l.empty()) {
        left = new TreeNode;
        left->submesh = l;
        left->subdivide();
    }

    if (!r.empty()) {
        right = new TreeNode;
        right->submesh = r;
        right->subdivide();
    }
}

void RaytraceableOptiMesh::TreeNode::cache()
{
    cached.clear();
    for (const Triangle3dd &triangle : submesh)
    {
        cached.push_back(triangle.toPlaneFrame());
    }
    if (left  != NULL)  left->cache();
    if (right != NULL) right->cache();
    if (middle != NULL) middle->cache();

}

int RaytraceableOptiMesh::TreeNode::childCount()
{
    int sum = 1;
    if (left) {
        sum += left->childCount();
    }
    if (right) {
        sum += right->childCount();
    }
    if (middle) {
        sum += middle->childCount();
    }
    return sum;
}

int RaytraceableOptiMesh::TreeNode::triangleCount()
{
    int sum = submesh.size();
    if (left) {
        sum += left->triangleCount();
    }
    if (right) {
        sum += right->triangleCount();
    }
    if (middle) {
        sum += middle->triangleCount();
    }
    return sum;
}

void RaytraceableOptiMesh::TreeNode::dumpToMesh(Mesh3D &mesh, int depth, bool plane, bool volume)
{
    mesh.addIcoSphere(bound, 3);

    if (left)   left  ->dumpToMesh(mesh, depth + 1, plane, volume);
    if (right)  right ->dumpToMesh(mesh, depth + 1, plane, volume);
    if (middle) middle->dumpToMesh(mesh, depth + 1, plane, volume);


}

void RaytraceableOptiMesh::optimize()
{
    delete_safe(opt);
    opt = new TreeNode();
    for (size_t i = 0; i < mMesh->faces.size(); i++)
    {
        Triangle3dd triangle = mMesh->getFaceAsTrinagle(i);
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
