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


    void switchColor(bool on = true)
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

    RGBColor currentColor;
    Matrix44 currentTransform;

/* Methods */


    void setCentral(Vector3dd _central)
    {
        centralPoint = _central;
        hasCentral = true;
    }


    void addAOB(Vector3dd corner1, Vector3dd corner2, bool addFaces = true);
    void addAOB(const AxisAlignedBoxParameters &box , bool addFaces = true);
    void addAOB(const AxisAlignedBox3d &box         , bool addFaces = true);

    int addPoint(Vector3dd point);

    void addLine(Vector3dd point1, Vector3dd point2);
    void addTriangle(Vector3dd point1, Vector3dd point2, Vector3dd point3);

    void addSphere(Vector3dd center, double radius, int step);

    void addCamera(const CameraIntrinsics &cam, double len);

    void add2AxisEllipse  (const EllipticalApproximation3d &approx);
    void addMatrixSurface (double *data, int h, int w);

    /* For abstract painter */
    typedef int InternalElementType;
    void drawLine(double x1, double y1, double x2, double y2, int);


#if 0
    void addTruncatedCone(double r1, double r2, double length, int steps = 16);
#endif

    void dumpPLY(ostream &out);
    void transform (const Matrix44 &matrix);
    Mesh3D transformed(const Matrix44 &matrix);

    void add(const Mesh3D &other);

private:
    virtual void addEdge(const Vector2d32 &edge);
    virtual void addVertex(const Vector3dd &vector);
    virtual void addFace(const Vector3d32 &faceId);


    //, Vector2dd &texCoord
public:
    virtual ~Mesh3D() {}
};



} /* namespace corecvs */
/* EOF */
