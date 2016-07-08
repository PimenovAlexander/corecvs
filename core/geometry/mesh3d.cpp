/**
 * \file mesh3d.cpp
 *
 * \date Dec 13, 2012
 **/
#include <fstream>

#include "mathUtils.h"      // M_PI
#include "abstractPainter.h"
#include "mesh3d.h"

namespace corecvs {

void Mesh3D::switchColor(bool on)
{
    if (hasColor == on)
        return;
    if (on) {
        vertexesColor.resize(vertexes.size(), currentColor);
        edgesColor   .resize(edges   .size(), currentColor);
        facesColor   .resize(faces   .size(), currentColor);
    } else {
        vertexesColor.clear();
        edgesColor.clear();
        facesColor.clear();
    }
    hasColor = on;
}

void Mesh3D::setColor(const RGBColor &color)
{
    currentColor = color;
}

void Mesh3D::mulTransform(const Matrix33 &transform)
{
    transformStack.push_back(currentTransform);
    currentTransform = currentTransform * Matrix44(transform);
}

void Mesh3D::mulTransform(const Matrix44 &transform)
{
    transformStack.push_back(currentTransform);
    currentTransform = currentTransform * transform;
}

void Mesh3D::popTransform()
{
    if (transformStack.empty()) {
        SYNC_PRINT(("Mesh3D::popTransform(): Poping on empty stack\n"));
        return;
    }
    currentTransform = transformStack.back();
    transformStack.pop_back();
}

void Mesh3D::setCentral(Vector3dd _central)
{
    centralPoint = _central;
    hasCentral = true;
}

void Mesh3D::addOrts(double length, bool captions)
{
    setColor(RGBColor::Red());
    addLine(Vector3dd(0.0), Vector3dd::OrtX() * length);
    setColor(RGBColor::Green());
    addLine(Vector3dd(0.0), Vector3dd::OrtY() * length);
    setColor(RGBColor::Blue());
    addLine(Vector3dd(0.0), Vector3dd::OrtZ() * length);

    /* Font Scale */
    Matrix44 fs = Matrix44::Scale(1.0 / 20.0) * Matrix44(Matrix33::MirrorXZ());

    /* Scale to length */
    Matrix44 tl = Matrix44::Scale(length / 20.0);


    AbstractPainter<Mesh3D> p(this);
    if (captions) {
        setColor(RGBColor::Red());
        mulTransform(tl * Matrix44::Shift(18.0, 0.2, 0.0) * fs);
        p.drawFormatVector(length, 0, 0, 1, "X");
        popTransform();

        mulTransform(tl * Matrix44(Matrix33::RotationX(degToRad(90.0))) * Matrix44::Shift(18.0, 0.2, 0.0)  * fs);
        p.drawFormatVector(length, 0, 0, 1, "X");
        popTransform();

        setColor(RGBColor::Green());
        mulTransform(tl * Matrix44::Shift(0, 18.0, 0.0) * fs);
        p.drawFormatVector(length, 0, 0, 1, "Y");
        popTransform();

        mulTransform(tl * Matrix44(Matrix33::RotationY(degToRad(-90.0))) * Matrix44::Shift(0, 18.0, 0.0) * fs);
        p.drawFormatVector(length, 0, 0, 1, "Y");
        popTransform();

        setColor(RGBColor::Blue());
        mulTransform(tl * Matrix44::Shift(0.0, 0.2, 20.0) * Matrix44(Matrix33::RotationY(degToRad(90.0))) * fs);
        p.drawFormatVector(length, 0, 0, 1, "Z");
        popTransform();

        mulTransform(tl * Matrix44(Matrix33::RotationZ(degToRad(-90.0))) * Matrix44::Shift(0.0, 0.2, 20.0) * Matrix44(Matrix33::RotationY(degToRad(90.0))) * fs);
        p.drawFormatVector(length, 0, 0, 1, "Z");
        popTransform();

    }

}

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
    return (int)vertexes.size() - 1;
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

void Mesh3D::addTriangle(const Triangle3dd &triangle)
{
    addTriangle(triangle.p1(), triangle.p2(), triangle.p3());
}

Triangle3dd Mesh3D::getFaceAsTrinagle(size_t number)
{
    Vector3d32 facei = faces[number];
    return Triangle3dd(vertexes[facei[0]], vertexes[facei[1]], vertexes[facei[2]]);
}

void Mesh3D::addSphere(Vector3dd center, double radius, int step)
{
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    double dphy =     M_PI / step ;
    double dpsi = 2 * M_PI / step ;

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

void Mesh3D::addCylinder(Vector3dd center, double radius, double height, int step, double phase)
{
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

    double dpsi = 2 * M_PI / step ;
    height = height / 2.0;

    addVertex(center + Vector3dd(0.0, 0.0, -height));
    addVertex(center + Vector3dd(0.0, 0.0,  height));


    for (int i = 0; i < step; i++)
    {
         double psi = dpsi * i + phase;
         addVertex(center + Vector3dd::FromCylindrical(psi, radius, -height));
         addVertex(center + Vector3dd::FromCylindrical(psi, radius,  height));
    }

    for (int i = 0; i < step; i++)
    {
        int i1 = 2 * ( i            ) + 2;
        int i2 = 2 * ((i + 1) % step) + 2;
        addFace(Vector3d32(0, i2    , i1    ) + startId); // Top cap

        addFace(Vector3d32(1, i1 + 1, i2 + 1) + startId); // Bottom cap

        addFace(Vector3d32(i1, i2    , i2 + 1) + startId); // Side
        addFace(Vector3d32(i1, i2 + 1, i1 + 1) + startId); //
    }
}

/**
 *   https://en.wikipedia.org/wiki/Regular_icosahedron#Cartesian_coordinates
 **/
void Mesh3D::addIcoSphere(Vector3dd center, double radius, int step)
{
    //double scaler = radius * sqrt(5);
    int vectorIndex = (int)vertexes.size();
    Vector3d32 startId(vectorIndex, vectorIndex, vectorIndex);

#if 0
    addVertex(center + Vector3dd(0.0,  1.0,  M_PHI));  // 0
    addVertex(center + Vector3dd(0.0, -1.0,  M_PHI));  // 1
    addVertex(center + Vector3dd(0.0,  1.0, -M_PHI));  // 2
    addVertex(center + Vector3dd(0.0, -1.0, -M_PHI));  // 3

    addVertex(center + Vector3dd( M_PHI,  0.0,  1.0)); // 4
    addVertex(center + Vector3dd(-M_PHI,  0.0,  1.0)); // 5
    addVertex(center + Vector3dd( M_PHI,  0.0, -1.0)); // 6
    addVertex(center + Vector3dd(-M_PHI,  0.0, -1.0)); // 7

    addVertex(center + Vector3dd( 1.0,  M_PHI,  0.0)); // 8
    addVertex(center + Vector3dd(-1.0,  M_PHI,  0.0)); // 9
    addVertex(center + Vector3dd( 1.0, -M_PHI,  0.0)); // 10
    addVertex(center + Vector3dd(-1.0, -M_PHI,  0.0)); // 11
    /**/

    /* and diagonal */
    addFace(startId + Vector3d32(0, 4, 8));
    addFace(startId + Vector3d32(1, 4, 8));
    addFace(startId + Vector3d32(0, 4, 8));
    addFace(startId + Vector3d32(0, 4, 8));

    addFace(startId + Vector3d32(0, 4, 8));
    addFace(startId + Vector3d32(0, 4, 8));
    addFace(startId + Vector3d32(0, 4, 8));
    addFace(startId + Vector3d32(0, 4, 8));


    /*addFace(startId + Vector3d32(0, 1, 4));
    addFace(startId + Vector3d32(0, 1, 5));

    addFace(startId + Vector3d32(2, 3, 6));
    addFace(startId + Vector3d32(2, 3, 7));


    addFace(startId + Vector3d32(4, 6,  8));
    addFace(startId + Vector3d32(4, 6, 10));

    addFace(startId + Vector3d32(5, 7,  9));
    addFace(startId + Vector3d32(5, 7, 11));


    addFace(startId + Vector3d32(8, 9,  0));
    addFace(startId + Vector3d32(8, 9,  2));

    addFace(startId + Vector3d32(10, 11, 1));
    addFace(startId + Vector3d32(10, 11, 3));*/
#else


    double level = atan(0.5);
    double da = M_PI * 2 / 5;
    double len = sqrt(1 - level * level);

    addVertex(center + Vector3dd(0.0,  0.0, 1.0) * radius);
    for (int i = 0; i < 5; i++) {
        double angle = da * i;
        addVertex(center + Vector3dd(cos(angle) * len, sin(angle) * len, level) * radius);
    }
    for (int i = 0; i < 5; i++) {
         double angle = da * i + (da / 2);
         addVertex(center + Vector3dd(cos(angle) * len, sin(angle) * len, -level) * radius);
    }
    addVertex(center + Vector3dd(0.0,  0.0, -1.0) * radius);

    /*Adding faces */
    static int ROUND_1 = 1;
    static int ROUND_2 = 1 + 5;
    static int LAST_P = 1 + 5 + 5;
    int primaryIndex = (int)faces.size();


     for (int i = 0; i < 5; i++) {
         addFace(startId + Vector3d32(0, i + ROUND_1, ((i + 1) % 5) + ROUND_1));
     }

     for (int i = 0; i < 5; i++) {
         int j = ((i + 1) % 5);
         addFace(startId + Vector3d32(j + ROUND_1, i + ROUND_1, i + ROUND_2));
         addFace(startId + Vector3d32(i + ROUND_2, j + ROUND_2, j + ROUND_1));
     }

     for (int i = 0; i < 5; i++) {
         addFace(startId + Vector3d32(LAST_P, ((i + 1) % 5) + ROUND_2, i + ROUND_2));
     }

     /*No we start the subdivision*/

     int faceIndex = primaryIndex;

     for (int stage = 0; stage < step; stage ++)
     {
         /*Take every face */

         int lastIndex = (int)faces.size();
         for (int f = faceIndex; f < lastIndex; f++)
         {
             Vector3d32 face = faces[f];

             /**/
             int startId = (int)vertexes.size();

             for (int k = 0; k < 3; k++)
             {
                 int startid = face[k];
                 int endid   = face[(k + 1) % 3];

                 /*if (endid < startid) continue;*/

                Vector3dd startvert = vertexes[startid];
                Vector3dd endvert   = vertexes[endid];

                Vector3dd add = (startvert + endvert) / 2.0;

                add = center + (add - center).normalised() * radius;

                addVertex(add);
             }
             addFace(Vector3d32(face[0], startId, startId + 2));
             addFace(Vector3d32(startId, face[1], startId + 1));
             addFace(Vector3d32(startId + 1, face[2], startId + 2));

             addFace(Vector3d32(startId, startId + 1, startId + 2));
         }
         faceIndex = lastIndex;
     }

     faces.erase(faces.begin() + primaryIndex, faces.begin() + faceIndex);
     if (hasColor) {
         facesColor.erase(facesColor.begin() + primaryIndex, facesColor.begin() + faceIndex);
     }



#endif

}

void Mesh3D::addCircle(const Circle3d &circle, int step)
{
    Vector3dd ort1;
    Vector3dd ort2;
    circle.normal.orthogonal(ort1, ort2);

    double dphy = 2 * M_PI / step ;
    for (int i = 0; i < step; i++)
    {
        double phi  = dphy * i;
        double phi1 = dphy * (i + 1);
        addLine(circle.c + (ort1 * sin(phi ) + ort2 * cos(phi )) * circle.r,
                circle.c + (ort1 * sin(phi1) + ort2 * cos(phi1)) * circle.r);
    }
}

void Mesh3D::drawCircle(Vector3dd center, double radius, int step, Vector3dd normal)
{
    int vectorIndex = (int)vertexes.size();
    Vector2d32 startIdE(vectorIndex, vectorIndex);
    Vector3d32 startIdF(vectorIndex, vectorIndex, vectorIndex);

    double psi = 2 * M_PI / step ;
    Vector3dd ort1, ort2;

    normal.orthogonal(ort1,ort2);

    for (int i = 0; i < step; i++)
    {
        addVertex(radius * (sin(psi * i) * ort1 + cos(psi * i) * ort2) + center);
    }
    addVertex(center);

    for (int i = 0; i < step; i++)
    {
        addEdge(startIdE + Vector2d32(i, (i + 1) % step));
    }

    for (int i = 0; i < step; i++)
    {
        addFace(startIdF + Vector3d32(i, (i + 1) % step, step));
    }
}

void Mesh3D::addSphere(const Sphere3d &sphere, int step)
{
    addSphere(sphere.c, sphere.r, step);
}

void Mesh3D::addIcoSphere(const Sphere3d &sphere, int step)
{
    addIcoSphere(sphere.c, sphere.r, step);
}

void Mesh3D::addCamera(const CameraIntrinsicsLegacy &cam, double len)
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

void Mesh3D::clear()
{
    vertexes.clear();
    faces.clear();
    edges.clear();

    textureCoords.clear();

    vertexesColor.clear();
    facesColor.clear();
    edgesColor.clear();
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

int Mesh3D::dumpPLY(ostream &out)
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
    return 0;
}

int Mesh3D::dumpPLY(const std::string &filename)
{
    std::ofstream out(filename, std::ios::out);
    if (out.bad())
    {
        SYNC_PRINT(("Mesh3D::dumpPLY(%s): could not save\n", filename.c_str()));
        return 1;
    }

    dumpPLY(out);
    out.close();
    return 0;
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

AxisAlignedBox3d Mesh3D::getBoundingBox()
{
    Vector3dd minP = Vector3dd(numeric_limits<double>::max());
    Vector3dd maxP = Vector3dd(numeric_limits<double>::lowest());

    for (size_t i = 0; i < vertexes.size(); i++)
    {
        for (int j = 0; j < Vector3dd::LENGTH; j++)
        {
            if (minP[j] < vertexes[i][j]) minP[j] = vertexes[i][j];
            if (maxP[j] > vertexes[i][j]) maxP[j] = vertexes[i][j];
        }
    }
    return AxisAlignedBox3d(minP, maxP);
}

void Mesh3D::add(const Mesh3D &other, bool preserveColor)
{
    int newZero = (int)vertexes.size();
    vertexes.reserve(vertexes.size() + other.vertexes.size());
    faces.reserve(faces.size() + other.faces.size());
    edges.reserve(edges.size() + other.edges.size());

    RGBColor backup = currentColor;
    preserveColor = preserveColor & other.hasColor;

    for (unsigned i = 0; i < other.vertexes.size(); i++)
    {
        if (preserveColor)
            currentColor = other.vertexesColor[i];

        addVertex(other.vertexes[i]);
    }

    for (unsigned i = 0; i < other.faces.size(); i++)
    {
        if (preserveColor)
            currentColor = other.facesColor[i];

        addFace(other.faces[i] + Vector3d32(newZero, newZero, newZero));
    }

    for (unsigned i = 0; i < other.edges.size(); i++)
    {
        if (preserveColor)
            currentColor = other.edgesColor[i];

        addEdge(other.edges[i] + Vector2d32(newZero, newZero));
    }

    currentColor = backup;
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

void Mesh3D::fillTestScene()
{

    switchColor();
    /* Vertex sphere */
    currentColor = RGBColor::Yellow();
    addSphere(Vector3dd(0), 50, 10);

    /* Face sphere */
    //int colorStart = (int)facesColor.size();

    currentColor = RGBColor::Red();

    addIcoSphere(Vector3dd(100.0,0.0,100.0), 50.0, 1);
    //int colorEnd = facesColor.size();

    /*for (int c = colorStart; c < colorEnd; c++)
    {
        facesColor[c] = RGBColor::rainbow(lerpLimit(0.0, 1.0, c, colorStart, colorEnd));
    }*/

    /* Edge box */
    currentColor = RGBColor::Blue();
    addAOB(Vector3dd(40.0,10.0,-40.0), Vector3dd(70.0,30.0,20.0), false);
}

void Mesh3D::dumpInfo(ostream &out)
{
    out << "Mesh3D::dumpInfo():" << endl;
    out << " Edges   :" << edges.size() << endl;
    out << " Vertexes:" << vertexes.size() << endl;
    out << " Faces   :" << faces.size() << endl;
    out << " Bounding box " << getBoundingBox() << endl;
    if (hasColor) {
        out << " -Edges    colors:" << edgesColor.size() << endl;
        out << " -Vertexes colors:" << vertexesColor.size() << endl;
        out << " -Faces    colors:" << facesColor.size() << endl;
    }

}

} /* namespace corecvs */
