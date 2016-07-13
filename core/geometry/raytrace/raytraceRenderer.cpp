#include "raytraceRenderer.h"

const double RaytraceableSphere::EPSILON = 0.000001;


RaytraceRenderer::RaytraceRenderer()
{

}

void RaytraceRenderer::trace(RayIntersection &intersection)
{
    // cout << "RaytraceRenderer::trace():" << intersection.ray << " " << intersection.depth << " " << intersection.weight << endl;
    bool hasInter = object->intersect(intersection);
    if (!hasInter || intersection.object == NULL) {
        // cout << "No intersection" << endl;
        return;
    }

    Raytraceable *obj = intersection.object;

    obj->normal(intersection.ray.getPoint(intersection.t), intersection.normal);

    if ((intersection.normal & intersection.ray.a) > 0 ) {
        intersection.normal = -intersection.normal;
    }

    if (obj->material == NULL)
        intersection.ownColor = obj->color;
    else {
        intersection.ownColor = TraceColor::Zero();
        obj->material->getColor(intersection, *this);
    }

    // SYNC_PRINT(("Has intersection!\n"));
}


void RaytraceRenderer::trace(RGB24Buffer *buffer)
{
    energy = new AbstractBuffer<TraceColor>(buffer->getSize());
    markup = new AbstractBuffer<int>(buffer->getSize());

    int inc = 0;

    parallelable_for(0, buffer->h, [&](const BlockedRange<int>& r )
        {
            for (int i = r.begin() ; i < r.end(); i++)
            {
                for (int j = 0; j < buffer->w; j++)
                {
                    currentX = j;
                    currentY = i;
                    Vector2dd pixel(j, i);
                    Ray3d ray = Ray3d(intrisics.reverse(pixel), Vector3dd::Zero());
                    ray = position * ray;

                    ray.normalise();

                    RayIntersection intersection;
                    intersection.ray = ray;
                    intersection.weight = 1.0;
                    intersection.depth = 0;
                    trace(intersection);
                    if (intersection.object != NULL) {
                        energy->element(i, j) = intersection.ownColor;
                    }
                }
                SYNC_PRINT(("\r[%d]", inc));
                inc++;
            }
        }

    );

    /*for (int i = 0; i < buffer->h; i++)
    {
        for (int j = 0; j < buffer->w; j++)
        {
            currentX = j;
            currentY = i;
            Vector2dd pixel(j, i);
            Ray3d ray = Ray3d(intrisics.reverse(pixel), Vector3dd::Zero());
            ray.normalise();

            RayIntersection intersection;
            intersection.ray = ray;
            intersection.weight = 1.0;
            intersection.depth = 0;
            trace(intersection);
            if (intersection.object != NULL) {
                energy->element(i, j) = intersection.ownColor;
            }
            SYNC_PRINT(("\r[%d %d]", i, j));
        }
    }*/

    for (int i = 0; i < buffer->h; i++)
    {
        for (int j = 0; j < buffer->w; j++)
        {
            buffer->element(i, j) = RGBColor::FromDouble(energy->element(i,j) / 2.1);
            if (markup->element(i, j) != 0)
            {
                buffer->element(i, j) = RGBColor::Red();
            }
        }
    }

    printf("\n");

    delete_safe(energy);
    delete_safe(markup);
}


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

Raytraceable::~Raytraceable()
{

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

void RaytraceableMaterial::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
{

    ray.computeBaseRays();
    Vector3dd intersection = ray.getPoint();
    // cout << "RaytraceableMaterial::getColor() : " << intersection << endl;


    /* Recursive calls */

    if (ray.depth < 4 && ray.weight > 0.01)
    {
        if (reflCoef != 0.0) {
            RayIntersection reflected;
            reflected.ray = ray.reflection;
            reflected.ray.a.normalise();
            reflected.weight = ray.weight * reflCoef;
            reflected.depth = ray.depth + 1;
            reflected.ownColor = TraceColor::Zero();

            renderer.trace(reflected);
            ray.ownColor += reflCoef * reflected.ownColor;

            // cout << "Relection Ray brings:" << reflected.ownColor << endl;
        }

/*        if (refrCoef != 0.0) {
            RayIntersection refracted;
            refracted.ray = ray.reflection;
            refracted.weight = ray.weight * refrCoef;
            refracted.depth = ray.depth + 1;

            renderer.trace(refracted);
            ray.ownColor += refrCoef * refracted.ownColor;
        }*/
    }

    ray.ownColor += renderer.ambient;
    ray.ownColor += ambient;

    for (RaytraceablePointLight *light: renderer.lights)
    {
        //cout << "Light сolor:" << light->color << " pos:" << light->position << endl;

        Vector3dd toLight = light->position  - intersection;

        //distance = -distance;

        RayIntersection lightRay;
        lightRay.depth  = 0;
        lightRay.weight = 1.0;

        lightRay.object = NULL;
        lightRay.ray.p = intersection;
        lightRay.ray.a = toLight;
        lightRay.ray.a.normalise();



        bool occluded = renderer.object->intersect(lightRay);
        if (occluded) {

            if (lightRay.t < (toLight.l2Metric() - 0.0001)) {
                /*SYNC_PRINT(("\nLight invisible %lf %lf\n", lightRay.t, (toLight.l2Metric() - 0.0001)));
                cout << "Processing: " << renderer.currentX << ","  << renderer.currentY << endl;
                cout << "Origin :" << intersection << endl;
                cout << "Outcoming :" << toLight << endl;
                cout << "Outcoming1:" << lightRay.ray.a << endl;

                cout << "Intersection at" << lightRay.getPoint() << endl;
                cout << "With " << lightRay.object->name << endl;
                renderer.markup->element(renderer.currentY, renderer.currentX) = 1;*/
                continue;
            }
        }

        if (!light->checkRay(lightRay)) {
            continue;
        }

        // SYNC_PRINT(("Light visible: "));
        double attenuation = (1.0 / toLight.l2Metric());
        attenuation = 1.0;

        /* Diffuse part */
        double diffuseKoef = (ray.normal.normalised() & lightRay.ray.a.normalised());

        if (diffuseKoef < 0) diffuseKoef = 0.0;
        TraceColor diffusePart = light->color * attenuation * diffuseKoef * diffuse;


        /* Specular part */
        double specularKoef = pow(ray.reflection.a.normalised() & lightRay.ray.a.normalised(), specPower);

        if (specularKoef < 0) specularKoef = 0.0;
        TraceColor specularPart = light->color * attenuation * specularKoef * specular;

        // cout << "Light " << diffusePart  << "(" << diffuseKoef << ") ";
        // cout             << specularPart << "(" << specularKoef << ") " << endl;

        ray.ownColor += diffusePart;
        ray.ownColor += specularPart;


    }
}

void RayIntersection::computeBaseRays()
{
    Vector3dd intersection = getPoint();
    normal.normalised();

    reflection.p = intersection;
    Vector3dd dir = ray.a.normalised();
    reflection.a = dir - 2 * normal * (normal & dir);
    reflection.a.normalise();

    refraction.p = intersection;
    refraction.a = ray.a;
    refraction.a.normalise();
}

Vector3dd RayIntersection::getPoint()
{
    return ray.getPoint(t);
}

void RaytraceableChessMaterial::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
{
    Vector3dd intersection = ray.getPoint();
    intersection = intersection / 10.0;

    bool b1;
    int vx = (int)intersection.x();
    b1 = (vx % 2);

    bool b2;
    int vy = (int)intersection.y();
    b2 = (vy % 2);

    bool b3;
    int vz = (int)intersection.z();
    b3 = (vz % 2);

    bool white = (((int)intersection.x()) % 2) ^ (((int)intersection.y()) % 2) ^ (((int)intersection.z()) % 2);
    ray.ownColor = (!b1 ^  !b2 ^ !b3) ? TraceColor::Zero() : RGBColor::White().toDouble();
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
//    SYNC_PRINT(("RaytraceableMesh::intersect(): entered\n"));

    for (int i = 0; i < mMesh->faces.size(); i++)
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

//    SYNC_PRINT(("RaytraceableMesh::intersect(): passed\n"));

    if (best.t == std::numeric_limits<double>::max()) {
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
