#pragma once

/**
 * \file mesh3d.h
 *
 * \date Dec 13, 2012
 **/

#include "generated/axisAlignedBoxParameters.h"
#include "vector3d.h"
#include "matrix33.h"
#include "matrix44.h"
#include "cameraParameters.h"
#include "axisAlignedBox.h"
#include "rgbColor.h"
#include "ellipticalApproximation.h"

#include "conic.h"

namespace corecvs
{

/**
 *  This class is overcompliced. Break it into several ones
 **/
class Mesh3D {
public:
    friend class PLYLoader;
    friend class STLLoader;

    Mesh3D() :
        centralPoint(0.0),
        hasCentral(false),

        hasTexCoords(false),
        hasColor(false),

        currentColor(RGBColor::Black()),
        currentTransform(1.0)
    {}


    Vector3dd  centralPoint;
    bool hasCentral;

/* Data that is stored */
    bool hasTexCoords;
    bool hasColor;

    /** Vertexes that from the mesh (faces or edges or noconnected) */
    vector<Vector3dd>  vertexes;
    vector<Vector3d32> faces;
    vector<Vector2d32> edges;

    /* Texture Coords Channel */
    vector<Vector2dd>  textureCoords;

    /* RGB Colors */
    vector<RGBColor> vertexesColor;
    vector<RGBColor> facesColor;
    vector<RGBColor> edgesColor;

/*  Current state */

    void switchColor(bool on = true);
    void setColor(const RGBColor &color);


    RGBColor currentColor;
    Matrix44 currentTransform;

    vector<Matrix44> transformStack;

    void mulTransform(const Matrix33 &transform);
    void mulTransform(const Matrix44 &transform);
    void popTransform();


/* Methods */


    void setCentral(Vector3dd _central);

    void addOrts(double length = 1.0, bool captions = false);


    void addAOB(Vector3dd corner1, Vector3dd corner2, bool addFaces = true);
    void addAOB(const AxisAlignedBoxParameters &box , bool addFaces = true);
    void addAOB(const AxisAlignedBox3d &box         , bool addFaces = true);

    int addPoint(Vector3dd point);

    void addLine(Vector3dd point1, Vector3dd point2);
    void addTriangle(Vector3dd point1, Vector3dd point2, Vector3dd point3);

    void addSphere    (Vector3dd center, double radius, int step);
    void addCylinder  (Vector3dd center, double radius, double height, int step = 20, double phase = 0.0);

    void addIcoSphere(Vector3dd center, double radius, int step = 1);

    void addCircle   (const Circle3d &circle, int step = 20);

    void addSphere   (const Sphere3d &sphere, int step = 20);
    void addIcoSphere(const Sphere3d &sphere, int step = 1);

    void addCamera(const CameraIntrinsicsLegacy &cam, double len);


    void add2AxisEllipse  (const EllipticalApproximation3d &approx);
    void addMatrixSurface (double *data, int h, int w);

    void clear();

    /* For abstract painter */
    typedef int InternalElementType;
    void drawLine(double x1, double y1, double x2, double y2, int);


#if 0
    void addTruncatedCone(double r1, double r2, double length, int steps = 16);
#endif

    void dumpPLY(ostream &out);
    int  dumpPLY(const string &filename);


    void transform (const Matrix44 &matrix);
    Mesh3D transformed(const Matrix44 &matrix);

    AxisAlignedBox3d getBoundingBox();

    void add(const Mesh3D &other, bool preserveColor = false);

private:
    virtual void addEdge(const Vector2d32 &edge);
    virtual void addVertex(const Vector3dd &vector);
    virtual void addFace(const Vector3d32 &faceId);


    //, Vector2dd &texCoord
public:
    virtual ~Mesh3D() {}

    void fillTestScene();
};



} /* namespace corecvs */
/* EOF */
