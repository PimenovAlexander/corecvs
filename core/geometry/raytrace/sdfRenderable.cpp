#include "geometry/raytrace/sdfRenderable.h"

SDFRenderable::SDFRenderable()
{

}

bool SDFRenderable::intersect(RayIntersection &intersection)
{
    intersection.object = NULL;

    Vector3dd walk = intersection.ray.p;
    double l = 0.0;

    static const int LIMIT  = 100;

    for (int i = 0; i < LIMIT; i++)
    {
        double d = F(walk);


        if (fabs(d) < 0.00000001) {

            double delta = 1e-7;
            Vector3dd normal;
            normal.x() = F(walk + Vector3dd::OrtX() * delta) - d;
            normal.y() = F(walk + Vector3dd::OrtY() * delta) - d;
            normal.z() = F(walk + Vector3dd::OrtZ() * delta) - d;
            normal /= delta;
            normal.normalise();
            intersection.object = this;
            intersection.normal = normal;
            intersection.t = l;

            //SYNC_PRINT(("d=%lf (%lf) (%lf %lf %lf)\n", l, d, normal.x(), normal.y(), normal.z()));
            return true;
        }
        l += fabs(d);
        if (fabs(d) > 1e10) {
            return false;
        }
        walk = intersection.ray.getPoint(l);
    }

    return false;
}

void SDFRenderable::normal(const Vector3dd &/*vector*/, Vector3dd &/*normal*/)
{

}

bool SDFRenderable::inside(Vector3dd &point)
{
    return F(point) > 0;
}

