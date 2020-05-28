#include <random>

#include "geometry/raytrace/raytraceRenderer.h"
#include "utils/preciseTimer.h"
#include "fileformats/bmpLoader.h"
#include <random>

RaytraceRenderer::RaytraceRenderer()
{

}

RaytraceRenderer::~RaytraceRenderer()
{
    delete_safe(projection);
}

void RaytraceRenderer::trace(RayIntersection &intersection)
{
    // cout << "RaytraceRenderer::trace():" << intersection.ray << " " << intersection.depth << " " << intersection.weight << endl;
    bool hasInter = false;
    if (object) {
        hasInter = object->intersect(intersection);
    }

    if (!hasInter || intersection.object == NULL) {
        // cout << "No intersection" << endl;
        if (sky != NULL) {
            sky->getColor(intersection, *this);
        } else {
            intersection.ownColor = TraceColor(0.0);
        }
        return;
    }

    Raytraceable *obj = intersection.object;

    obj->normal(intersection);

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

    PreciseTimer startTime = PreciseTimer::currentTime();
    cout << "Will render:" << buffer->getSize() << endl;

    if (!supersample)
    {
        parallelable_for(0, buffer->h, [&](const BlockedRange<int>& r )
            {
                for (int i = r.begin() ; i < r.end(); i++)
                {
                    for (int j = 0; j < buffer->w; j++)
                    {
                        currentX = j;
                        currentY = i;
                        Vector2dd pixel(j, i);
                        Ray3d ray = Ray3d(projection->reverse(pixel), Vector3dd::Zero());
                        ray = position * ray;

                        ray.normalise();

                        RayIntersection intersection;
                        intersection.ray = ray;
                        intersection.weight = 1.0;
                        intersection.depth = 0;
                        trace(intersection);
                        if (/*intersection.object != NULL*/ 1) {
                            energy->element(i, j) = intersection.ownColor;
                        } else {
                            //energy->element(i, j) = RGBColor::Cyan().toDouble();
                        }
                    }
                    SYNC_PRINT(("\r[%d / %d]", inc, buffer->h));
                    inc++;
                }
            }, parallel

        );
    } else {
        SYNC_PRINT(("This is a supersample render with %d random samples per pixel (%d rays)\n", sampleNum, sampleNum * buffer->h * buffer->w));
        parallelable_for(0, buffer->h, [&](const BlockedRange<int>& r )
            {                
                for (int i = r.begin() ; i < r.end(); i++)
                {
                    //std::minstd_rand rayscatter;
                    std::minstd_rand rayscatter(i);
                    std::uniform_real_distribution<double> dis(-0.5, 0.5);

                    for (int j = 0; j < buffer->w; j++)
                    {
                        TraceColor sumColor = TraceColor(0);

                        for (int sample = 0; sample < sampleNum; sample++)
                        {
                            currentX = j;
                            currentY = i;
                            Vector2dd pixel(j, i);
                            pixel.x() += dis(rayscatter);
                            pixel.y() += dis(rayscatter);

                            Ray3d ray = Ray3d(projection->reverse(pixel), Vector3dd::Zero());
                            ray = position * ray;
                            ray.normalise();



                            RayIntersection intersection;
                            intersection.ray = ray;
                            intersection.weight = 1.0;
                            intersection.depth = 0;
                            trace(intersection);
                            if (true) {
                                sumColor += intersection.ownColor;
                            } else {

                            }
                        }
                        energy->element(i, j) = sumColor / sampleNum;
                    }
                    SYNC_PRINT(("\rsupersample[%d / %d ]", inc, buffer->h));
                    inc++;
                }
            }, parallel

        );
    }

    pack(buffer, energy, markup);
    printf("\n");

    SYNC_PRINT(("Finished in:%5.2lfs\n", startTime.usecsToNow() / 1000.0 / 1000.0));

    delete_safe(energy);
    delete_safe(markup);
}

void RaytraceRenderer::traceFOV(RGB24Buffer *buffer, double apperture, double focus)
{
    energy = new ColorBuffer(buffer->getSize());
    markup = new MarkupType(buffer->getSize());

    int inc = 0;

    PreciseTimer startTime = PreciseTimer::currentTime();
    PreciseTimer lastDump = startTime;

    parallelable_for(0, buffer->h, [&](const BlockedRange<int>& r )
        {
            for (int i = r.begin() ; i < r.end(); i++)
            {
                //std::mt19937 rayscatter;
                std::minstd_rand rayscatter(i);
                std::uniform_real_distribution<double> dis(-0.5, 0.5);

                for (int j = 0; j < buffer->w; j++)
                {
                    TraceColor sumColor = TraceColor(0);

                    for (int sample = 0; sample < sampleNum; sample++)
                    {

                        currentX = j;
                        currentY = i;
                        Vector2dd pixel(j, i);

                        Ray3d ray = Ray3d(projection->reverse(pixel), Vector3dd::Zero());
                        ray = position * ray;
                        ray.normalise();

                        Vector3dd shift;
                        shift.y() = dis(rayscatter) * apperture;
                        shift.x() = dis(rayscatter) * apperture;
                        shift.z() = 0.0;

                        Vector3dd foc = ray.getPoint(focus);
                        Vector3dd st  = ray.p + shift;

                        Ray3d r((foc-st).normalised(), st);

                        RayIntersection intersection;
                        intersection.ray = r;
                        intersection.weight = 1.0;
                        intersection.depth = 0;
                        trace(intersection);
                        if (intersection.object != NULL) {
                            sumColor += intersection.ownColor;
                        } else {

                        }
                    }
                    energy->element(i, j) = sumColor / sampleNum;
                }


                inc++;

                double passed = startTime.usecsToNow();
                double left = passed / inc * (buffer->h - inc);
                SYNC_PRINT(("\rfocal[%d / %d] passed:%5.2lfs left %5.2lfs", inc, buffer->h, passed / 1000000.0, left / 1000000.0));

                if (traceProgress && (lastDump.usecsToNow() > 30 * 1000 * 1000))
                {
                     // Need mutex here...
                    RGB24Buffer *temp = new RGB24Buffer(buffer->getSize());
                    pack(temp, energy, markup);
                    BMPLoader().save("trace.bmp", temp);
                    lastDump = PreciseTimer::currentTime();
                    delete_safe(temp);
                }

            }
        }, parallel
    );

    pack(buffer, energy, markup);
    printf("\n");

    delete_safe(energy);
    delete_safe(markup);
}

void RaytraceRenderer::setProjection(CameraProjection *projection)
{
    delete_safe(this->projection);
    this->projection = projection;
}

void RaytraceRenderer::pack(RGB24Buffer *target, RaytraceRenderer::ColorBuffer *energy, RaytraceRenderer::MarkupType *markup)
{
    for (int i = 0; i < target->h; i++)
    {
        for (int j = 0; j < target->w; j++)
        {
            target->element(i, j) = RGBColor::FromDouble(energy->element(i,j) / 2.1);
            if (markup && markup->element(i, j) != 0)
            {
                target->element(i, j) = RGBColor::Red();
            }
        }
    }
}



void RaytraceableMaterial::getColor(RayIntersection &ray, RaytraceRenderer &renderer)
{

    //ray.computeBaseRays();
    Vector3dd intersection = ray.getPoint();
    // cout << "RaytraceableMaterial::getColor() : " << intersection << endl;


    /* Recursive calls */
    Ray3d reflectionRay = getReflection(ray);
    Ray3d refractionRay = getRefraction(ray);


    if (ray.depth <= renderer.maxDepth && ray.weight > renderer.minWeight)
    {
        if (reflCoef != 0.0)
        {
            RayIntersection reflected;
            reflected.ray = reflectionRay;
            reflected.ray.a.normalise();
            reflected.weight = ray.weight * reflCoef;
            reflected.depth = ray.depth + 1;
            reflected.ownColor = TraceColor::Zero();

            renderer.trace(reflected);
            ray.ownColor += reflCoef * reflected.ownColor;

            // cout << "Relection Ray brings:" << reflected.ownColor << endl;
        }

        if (refrCoef != 0.0 && refractionRay.a.l1Metric() != 0.0)
        {
            RayIntersection refracted;
            refracted.ray = refractionRay;
            refracted.ray.a.normalise();
            refracted.weight = ray.weight * refrCoef;
            refracted.depth = ray.depth + 1;
            refracted.ownColor = TraceColor::Zero();

            renderer.trace(refracted);
            ray.ownColor += refrCoef * refracted.ownColor;
        }
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
        lightRay.ray.p = lightRay.ray.getPoint(0.00001);



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
        double specBase = reflectionRay.a.normalised() & lightRay.ray.a.normalised();
        if (specBase < 0) specBase = 0.0;
        double specularKoef = pow(specBase, specPower);

        if (specularKoef < 0) specularKoef = 0.0;
        TraceColor specularPart = light->color * attenuation * specularKoef * specular;

        // cout << "Light " << diffusePart  << "(" << diffuseKoef << ") ";
        // cout             << specularPart << "(" << specularKoef << ") " << endl;

        ray.ownColor += diffusePart;
        ray.ownColor += specularPart;


    }
}

Ray3d RaytraceableMaterial::getReflection(RayIntersection &ray)
{
    Vector3dd intersection = ray.getPoint();
    Vector3dd normal = ray.normal.normalised();

    Ray3d reflection;

    reflection.p = intersection;
    Vector3dd dir = ray.ray.a.normalised();
    reflection.a = dir - 2 * normal * (normal & dir);
    reflection.a.normalise();
    return reflection;
}

Ray3d RaytraceableMaterial::getRefraction(RayIntersection &ray)
{
    Ray3d refraction;
    refraction.p = ray.getPoint();
    refraction.a = Vector3dd::Zero();
    Vector3dd dir = ray.ray.a.normalised();
    Vector3dd normal = ray.normal.normalised();

    double comp = -dir & normal;
    double N = comp > 0 ? opticalDens : -opticalDens;

    double disk = 1 + N * N * (comp * comp - 1);

    if (disk > 0)
    {
        refraction.a =  N * dir + (N * comp - sqrt(disk)) * normal;
    }
    return refraction;
}

Vector3dd RayIntersection::getPoint()
{
    return ray.getPoint(t);
}

void RaytraceableChessMaterial::getColor(RayIntersection &ray, RaytraceRenderer &/*renderer*/)
{
    Vector3dd intersection = ray.getPoint();
    intersection = intersection / cellSize;

    bool b1;
    int vx = (int)intersection.x();
    b1 = (vx % 2);

    bool b2;
    int vy = (int)intersection.y();
    b2 = (vy % 2);

    bool b3;
    int vz = (int)intersection.z();
    b3 = (vz % 2);

    //bool white = (((int)intersection.x()) % 2) ^ (((int)intersection.y()) % 2) ^ (((int)intersection.z()) % 2);
    ray.ownColor = (!b1 ^  !b2 ^ !b3) ? TraceColor::Zero() : RGBColor::White().toDouble();
}




void Raytraceable::normal(RayIntersection &/*intersection*/)
{

}

AxisAlignedBox3d Raytraceable::getBoundingBox()
{
    return AxisAlignedBox3d::AllSpace();
}

Raytraceable::~Raytraceable()
{

}

void RaytraceableSky::getColor(RayIntersection &ray, RaytraceRenderer &/*renderer*/)
{
    double v = ray.ray.a & direction;
    ray.ownColor = lerp(low, high, v, -1.0, 1.0);
}

