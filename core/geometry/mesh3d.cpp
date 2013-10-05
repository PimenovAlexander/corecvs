/**
 * \file mesh3d.cpp
 *
 * \date Dec 13, 2012
 **/
#include "mathUtils.h"      // M_PI
#include "mesh3d.h"

namespace corecvs {

void Mesh3D::addAOB(Vector3dd c1, Vector3dd c2)
{
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    vertexes.push_back(Vector3dd(c1.x(), c1.y(), c1.z()));
    vertexes.push_back(Vector3dd(c2.x(), c1.y(), c1.z()));
    vertexes.push_back(Vector3dd(c2.x(), c2.y(), c1.z()));
    vertexes.push_back(Vector3dd(c1.x(), c2.y(), c1.z()));

    vertexes.push_back(Vector3dd(c1.x(), c1.y(), c2.z()));
    vertexes.push_back(Vector3dd(c2.x(), c1.y(), c2.z()));
    vertexes.push_back(Vector3dd(c2.x(), c2.y(), c2.z()));
    vertexes.push_back(Vector3dd(c1.x(), c2.y(), c2.z()));

    faces.push_back(startId + Vector3d32(0, 1, 2));
    faces.push_back(startId + Vector3d32(0, 1, 2));
    faces.push_back(startId + Vector3d32(2, 3, 0));
    faces.push_back(startId + Vector3d32(7, 6, 5));
    faces.push_back(startId + Vector3d32(5, 4, 7));
    faces.push_back(startId + Vector3d32(0, 4, 5));
    faces.push_back(startId + Vector3d32(5, 1, 0));
    faces.push_back(startId + Vector3d32(1, 5, 6));
    faces.push_back(startId + Vector3d32(6, 2, 1));
    faces.push_back(startId + Vector3d32(2, 6, 7));
    faces.push_back(startId + Vector3d32(7, 3, 2));
    faces.push_back(startId + Vector3d32(3, 7, 4));
    faces.push_back(startId + Vector3d32(4, 0, 3));

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));
}

void Mesh3D::addAOB(const AxisAlignedBoxParameters &box)
{
    Vector3dd measure(box.width(), box.height(), box.depth());
    Vector3dd center (box.x()    , box.y()     , box.z    ());


    addAOB(center - measure / 2.0, center + measure / 2.0);
}

void Mesh3D::addAOB(const AxisAlignedBox3d &box)
{
    addAOB(box.low(), box.high());
}

void Mesh3D::addSphere(Vector3dd center, double radius, int step)
{
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    double dphy =     M_PI / (step + 1);
    double dpsi = 2 * M_PI / (step + 1);

    for (int i = 0; i < step; i++)
    {
        for (int j = 0; j < step; j++)
        {
            double phi = dphy * i;
            double psi = dpsi * j;

            double x = radius * sin(phi) * sin(psi);
            double y = radius * sin(phi) * cos(psi);
            double z = radius * cos(phi);
            vertexes.push_back(center + Vector3dd(x,y,z));
        }
    }
}

#if 0
void Mesh3D::addTruncatedCone(double r1, double r2, double length, int steps)
{
    int vectorIndex = vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    /*for (int i = 0; i < steps; i++)
    {
        double angle1 = (i / (double)steps * 2.0 * M_PI);
        double angle2 = (((i + 1) % steps) / (double)steps * 2.0 * M_PI);

        double sa1 = sin(angle1);
        double ca1 = cos(angle1);

        double sa2 = sin(angle2);
        double ca2 = cos(angle2);

        glVertex3d(r1 * sa1, r1 * ca1 ,-length);
        glVertex3d(r1 * sa2, r1 * ca2 ,-length);

        glVertex3d(r2 * sa1, r2 * ca1 ,-0.0);
        glVertex3d(r2 * sa2, r2 * ca2 ,-0.0);

        glVertex3d(r1 * sa1, r1 * ca1 ,-length);
        glVertex3d(r2 * sa1, r2 * ca1 ,-0.0);
    }*/
}
#endif

} /* namespace corecvs */
