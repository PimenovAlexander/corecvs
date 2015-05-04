/**
 * \file mesh3d.cpp
 *
 * \date Dec 13, 2012
 **/
#include "mathUtils.h"      // M_PI
#include "mesh3d.h"

namespace corecvs {

void Mesh3D::addAOB(Vector3dd c1, Vector3dd c2, bool addFaces)
{
    int vectorIndex = (int)vertexes.size();
    addVertex(Vector3dd(c1.x(), c1.y(), c1.z()));
    addVertex(Vector3dd(c2.x(), c1.y(), c1.z()));
    addVertex(Vector3dd(c2.x(), c2.y(), c1.z()));
    addVertex(Vector3dd(c1.x(), c2.y(), c1.z()));

    addVertex(Vector3dd(c1.x(), c1.y(), c2.z()));
    addVertex(Vector3dd(c2.x(), c1.y(), c2.z()));
    addVertex(Vector3dd(c2.x(), c2.y(), c2.z()));
    addVertex(Vector3dd(c1.x(), c2.y(), c2.z()));

    if (addFaces)
    {
        Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);
        addFace(startId + Vector3d32(0, 1, 2));
        addFace(startId + Vector3d32(0, 1, 2));
        addFace(startId + Vector3d32(2, 3, 0));
        addFace(startId + Vector3d32(7, 6, 5));
        addFace(startId + Vector3d32(5, 4, 7));
        addFace(startId + Vector3d32(0, 4, 5));
        addFace(startId + Vector3d32(5, 1, 0));
        addFace(startId + Vector3d32(1, 5, 6));
        addFace(startId + Vector3d32(6, 2, 1));
        addFace(startId + Vector3d32(2, 6, 7));
        addFace(startId + Vector3d32(7, 3, 2));
        addFace(startId + Vector3d32(3, 7, 4));
        addFace(startId + Vector3d32(4, 0, 3));
    } else {
        Vector2d32 startId(vectorIndex, vectorIndex);
        addEdge(startId +  Vector2d32(0, 1));
        addEdge(startId +  Vector2d32(1, 2));
        addEdge(startId +  Vector2d32(2, 3));
        addEdge(startId +  Vector2d32(3, 0));

        addEdge(startId +  Vector2d32(4, 5));
        addEdge(startId +  Vector2d32(5, 6));
        addEdge(startId +  Vector2d32(6, 7));
        addEdge(startId +  Vector2d32(7, 4));

        addEdge(startId +  Vector2d32(0, 4));
        addEdge(startId +  Vector2d32(1, 5));
        addEdge(startId +  Vector2d32(2, 6));
        addEdge(startId +  Vector2d32(3, 7));
    }

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));
}

void Mesh3D::addAOB(const AxisAlignedBoxParameters &box, bool addFaces)
{
    Vector3dd measure(box.width(), box.height(), box.depth());
    Vector3dd center (box.x()    , box.y()     , box.z    ());


    addAOB(center - measure / 2.0, center + measure / 2.0, addFaces);
}

void Mesh3D::addAOB(const AxisAlignedBox3d &box, bool addFaces)
{
    addAOB(box.low(), box.high(), addFaces);
}

int Mesh3D::addPoint(Vector3dd point)
{
     addVertex(point);
     return vertexes.size() - 1;
}

void Mesh3D::addLine(Vector3dd point1, Vector3dd point2)
{
    int vectorIndex = (int)vertexes.size();
    Vector2d32 startId(vectorIndex, vectorIndex);

    addVertex(point1);
    addVertex(point2);

    addEdge(startId + Vector2d32(0, 1));

}

void Mesh3D::addTriangle(Vector3dd point1, Vector3dd point2, Vector3dd point3)
{
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    addVertex(point1);
    addVertex(point2);
    addVertex(point3);

    addFace(startId + Vector3d32(0, 1, 2));
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
            addVertex(center + Vector3dd(x,y,z));
        }
    }
}

void Mesh3D::addCamera(const CameraIntrinsics &cam, double len)
{
    //double aspect = cam.

    double farPlane  = len;
    double alphaH = cam.getHFov();
    double alphaV = cam.getVFov();

    double x, y;
    x = farPlane  * tan (alphaH);
    y = farPlane  * tan (alphaV);

    int vectorIndex = (int)vertexes.size();
    Vector2d32 startId(vectorIndex, vectorIndex);

    addVertex(Vector3dd(0,0,0));
    addVertex(Vector3dd( x, y, farPlane));
    addVertex(Vector3dd( x,-y, farPlane));
    addVertex(Vector3dd(-x,-y, farPlane));
    addVertex(Vector3dd(-x, y, farPlane));

    addEdge(Vector2d32(0,1) + startId);
    addEdge(Vector2d32(0,2) + startId);
    addEdge(Vector2d32(0,3) + startId);
    addEdge(Vector2d32(0,4) + startId);

    addEdge(Vector2d32(1,2) + startId);
    addEdge(Vector2d32(2,3) + startId);
    addEdge(Vector2d32(3,4) + startId);
    addEdge(Vector2d32(4,1) + startId);

    //SYNC_PRINT(("This 0x%X. Edges %d", this, edges.size()));
    //for (unsigned i = 0; i < edges.size(); i++)
    //{
    //    SYNC_PRINT(("Edges %d - [%d - %d]\n", i, edges[i].x(), edges[i].y()));
    //}

}

void Mesh3D::add2AxisEllipse(const EllipticalApproximation3d &approx)
{
    if (approx.isEmpty() || approx.mAxes.empty()) {
        return;
    }

    int step = 20;
    double dpsi = 2 * M_PI / (step);

    Vector3dd center = approx.getCenter();

    Vector3dd prev;
    for (int i = 0; i <= step; i++)
    {
        double y = sin(dpsi * i);
        double x = cos(dpsi * i);
        Vector3dd curr = center +
               approx.mAxes[0] * sqrt(approx.mValues[0]) * x +
               approx.mAxes[1] * sqrt(approx.mValues[1]) * y;
        if (i != 0) {
            addLine(prev, curr);
        }
        prev = curr;
    }


}

void Mesh3D::addMatrixSurface(double *data, int h, int w)
{
    int vectorIndex = (int)vertexes.size();
    Vector2d32 startId(vectorIndex, vectorIndex);


    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            addVertex(Vector3dd(j, i, data[i * w + j]));
        }
    }

    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
            if (i > 0) {
                addEdge(Vector2d32(i * w + j, (i - 1) * w + j    ) + startId);
            }
            if (j > 0) {
                addEdge(Vector2d32(i * w + j,       i * w + j - 1) + startId);
            }

        }
    }
}

void Mesh3D::drawLine(double x1, double y1, double x2, double y2, int /*color*/)
{
    addLine(Vector3dd(x1, y1, 0.0), Vector3dd(x2, y2, 0.0));
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

void Mesh3D::dumpPLY(ostream &out)
{
    out << "ply" << std::endl;
    out << "format ascii 1.0" << std::endl;
    out << "comment made by ViMouse software" << std::endl;
    out << "comment This file is a saved stereo-reconstruction" << std::endl;
    out << "element vertex " << vertexes.size() << std::endl;
    out << "property float x" << std::endl;
    out << "property float y" << std::endl;
    out << "property float z" << std::endl;
    out << "property uchar red" << std::endl;
    out << "property uchar green" << std::endl;
    out << "property uchar blue" << std::endl;
    out << "element face " << faces.size() << std::endl;
    out << "property list uchar int vertex_index" << std::endl;
    if (hasColor) {
        out << "property uchar red" << std::endl;
        out << "property uchar green" << std::endl;
        out << "property uchar blue" << std::endl;
    }
    out << "element edge " << edges.size() << std::endl;
    out << "property int vertex1" << std::endl;
    out << "property int vertex2" << std::endl;
    if (hasColor) {
        out << "property uchar red" << std::endl;
        out << "property uchar green" << std::endl;
        out << "property uchar blue" << std::endl;
    }
    out << "end_header" << std::endl;

    for (unsigned i = 0; i < vertexes.size(); i++)
    {
        out << vertexes[i].x() << " "
            << vertexes[i].y() << " "
            << vertexes[i].z() << " ";
        if (hasColor) {
            out << (unsigned)(vertexesColor[i].r()) << " "
                << (unsigned)(vertexesColor[i].g()) << " "
                << (unsigned)(vertexesColor[i].b()) << std::endl;
        } else {
            out << (unsigned)(128) << " "
                << (unsigned)(128) << " "
                << (unsigned)(128) << std::endl;
        }
    }

    for (unsigned i = 0; i < faces.size(); i++)
    {
        out << "3 "
            << faces[i].x() << " "
            << faces[i].y() << " "
            << faces[i].z() << " ";
        if (hasColor) {
            out << (unsigned)(facesColor[i].r()) << " "
                << (unsigned)(facesColor[i].g()) << " "
                << (unsigned)(facesColor[i].b());
        }
        out << std::endl;
    }

    for (unsigned i = 0; i < edges.size(); i++)
    {

        out << edges[i].x() << " "
            << edges[i].y() << " ";
        if (hasColor) {
            out << (unsigned)(edgesColor[i].r()) << " "
                << (unsigned)(edgesColor[i].g()) << " "
                << (unsigned)(edgesColor[i].b());
        }
        out << std::endl;
    }

//    SYNC_PRINT(("This 0x%X. Edges %d", this, edges.size()));

}

void Mesh3D::transform(const Matrix44 &matrix)
{
    for (unsigned i = 0; i < vertexes.size(); i++)
    {
        vertexes[i] = matrix * vertexes[i];
    }
}

Mesh3D Mesh3D::transformed(const Matrix44 &matrix)
{
    Mesh3D toReturn;
    toReturn = *this;
    toReturn.transform(matrix);
    return toReturn;
}

void Mesh3D::add(const Mesh3D &other)
{
    int newZero = vertexes.size();
    vertexes.reserve(vertexes.size() + other.vertexes.size());
    faces.reserve(faces.size() + other.faces.size());
    edges.reserve(edges.size() + other.edges.size());

    for (unsigned i = 0; i < other.vertexes.size(); i++)
    {
        addVertex(other.vertexes[i]);
    }

    for (unsigned i = 0; i < other.faces.size(); i++)
    {
        addFace(other.faces[i] + Vector3d32(newZero, newZero, newZero));
    }

    for (unsigned i = 0; i < other.edges.size(); i++)
    {
        addEdge(other.edges[i] + Vector2d32(newZero, newZero));
    }
}



void Mesh3D::addEdge(const Vector2d32 &edge)
{
    edges.push_back(edge);
    if (hasColor) {
        edgesColor.push_back(currentColor);
    }
}

void Mesh3D::addVertex(const Vector3dd &vector)
{
    vertexes.push_back(currentTransform * vector);
    if (hasColor) {
        vertexesColor.push_back(currentColor);
    }
}

void Mesh3D::addFace(const Vector3d32 &faceId)
{
    faces.push_back(faceId);
    if (hasColor) {
        facesColor.push_back(currentColor);
    }
}



} /* namespace corecvs */
