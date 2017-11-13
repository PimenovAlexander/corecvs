#pragma once

/**
 * \file mesh3d.h
 *
 * \date Dec 13, 2012
 **/

#include "core/xml/generated/axisAlignedBoxParameters.h"
#include "core/math/vector/vector3d.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/matrix/matrix44.h"
#include "core/cammodel/cameraParameters.h"
#include "core/geometry/axisAlignedBox.h"
#include "core/buffers/rgb24/rgbColor.h"
#include "core/geometry/ellipticalApproximation.h"
#include "core/geometry/polygons.h"

#include "core/geometry/conic.h"

namespace corecvs
{

/**
 *  This class is overcomplicated. Break it into several ones
 **/
class Mesh3D {
public:
    friend class PLYLoader;
    friend class STLLoader;
    friend class OBJLoader;

    Mesh3D() :
        centralPoint(0.0),
        hasCentral(false),      
        hasColor(false),

        currentColor(RGBColor::Black()),
        currentTransform(1.0)
    {}


    Vector3dd  centralPoint;
    bool hasCentral;

/* Data that is stored */
    bool hasColor;

    /** Vertexes that from the mesh (faces or edges or noconnected) */
    vector<Vector3dd>  vertexes;
    vector<Vector3d32> faces;
    vector<Vector2d32> edges;

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

    /* All subsecquent draws would be as if origin is moved be affine transform */
    void mulTransform(const Affine3DQ &transform);

    void mulTransform(const Matrix33 &transform);
    void mulTransform(const Matrix44 &transform);
    void popTransform();


/* Methods */


    void setCentral(Vector3dd _central);

    void addOrts      (double length = 1.0, bool captions = false);
    void addPlaneFrame(const PlaneFrame &frame, double length = 1.0);


    virtual void addAOB(const Vector3dd &corner1, const Vector3dd &corner2, bool addFaces = true);
    void addAOB(const AxisAlignedBoxParameters &box , bool addFaces = true);
    void addAOB(const AxisAlignedBox3d &box         , bool addFaces = true);

    int addPoint(const Vector3dd &point);

    void addLine(const Vector3dd &point1, const Vector3dd &point2);
    void addTriangle(Vector3dd point1, Vector3dd point2, Vector3dd point3);
    void addTriangle(const Triangle3dd &triangle);
    void addFlatPolygon(const FlatPolygon &polygon);

    Triangle3dd getFaceAsTrinagle(size_t number);

    void addSphere    (Vector3dd center, double radius, int step = 10);

    /**
     *  Adds a cylinder to the Mesh.
     *
     *  \param center - center of the cylinder(center of the axis)
     *  \param radius - cylinder radius
     *
     **/
    void addCylinder  (Vector3dd center, double radius, double height, int step = 20, double phase = 0.0);

    void addIcoSphere(Vector3dd center, double radius, int step = 1);

    void addCircle   (const Circle3d &circle, int step = 20);
    void drawCircle  (Vector3dd center, double radius, int step=20, Vector3dd normal=Vector3dd(0, 0, 1));

    void addSphere   (const Sphere3d &sphere, int step = 20);
    void addIcoSphere(const Sphere3d &sphere, int step = 1);


    void addCamera(const CameraIntrinsicsLegacy &cam, double len);

    void add2AxisEllipse  (const EllipticalApproximation3d &approx);
    void addMatrixSurface (double *data, int h, int w);

    virtual void clear();

    /* For abstract painter */
    typedef int InternalElementType;
    void drawLine(double x1, double y1, double x2, double y2, int);


#if 0
    void addTruncatedCone(double r1, double r2, double length, int steps = 16);
#endif

    int dumpPLY(ostream &out);
    int dumpPLY(const string &filename);


    virtual void transform (const Matrix44 &matrix);
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

    virtual void fillTestScene();

    virtual void dumpInfo(ostream &out = std::cout);
};



} /* namespace corecvs */
/* EOF */
