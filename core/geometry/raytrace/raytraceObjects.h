#ifndef RAYTRACEOBJECTS_H
#define RAYTRACEOBJECTS_H

#include "geometry/raytrace/bspTree.h"
#include "geometry/mesh/mesh3DDecorated.h"
#include "geometry/raytrace/raytraceRenderer.h"
#include "geometry/raytrace/raytraceableNodeWrapper.h"

class RaytraceableTransform : public Raytraceable
{
public:
    Matrix44 mMatrix;
    Matrix44 mMatrixInv;

    Raytraceable *mObject;

    RaytraceableTransform(Raytraceable *object, const Matrix44 &matrix) :
        mMatrix(matrix),
        mObject(object)
    {
        mMatrixInv = mMatrix.inverted();
    }

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection)   override;
    virtual bool inside (Vector3dd &point)  override;

};

class RaytraceableSphere : public Raytraceable{
public:
    static const double EPSILON;

    bool flag;
    Sphere3d mSphere;

    RaytraceableSphere(const Sphere3d &sphere) :
        flag(false),
        mSphere(sphere)
    {
    }

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection)   override;
    virtual bool inside (Vector3dd &point)  override;
    virtual AxisAlignedBox3d getBoundingBox() override;

    // Raytraceable interface
public:
    virtual bool toMesh(Mesh3D &target) override;
};

class RaytraceableCylinder: public Raytraceable {
private:
    Vector3dd p;   /**< center of one of the faces */
    Matrix33 rotation; /**<   Stores cylinder orientation. World to "cylinder zero" tarnsformation  */

public:
    static const double EPSILON;

    bool flag;

    double r;      /**< cylinder radius */
    double h;      /**< cylinder height */

    RaytraceableCylinder() :
          p(Vector3dd::Zero())
        , rotation(Matrix33::RotationX(degToRad(90)))
        , flag(false)
    {}

    void setPosition(const Affine3DQ &affine);
    void setPosition(const Vector3dd &position);
    void setPosition(double x, double y, double z);

    void setRotation(const Matrix33 &rotation);
    Matrix33 getRotationToCylinder();


    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection)   override;
    virtual bool inside (Vector3dd &point)  override;    
    virtual AxisAlignedBox3d getBoundingBox() override;



    // Raytraceable interface
public:
    virtual bool toMesh(Mesh3D &) override;
};


class RaytraceablePlane : public Raytraceable {
public:
    static const double EPSILON;

    bool flag;
    Plane3d mPlane;

    RaytraceablePlane(const Plane3d &plane) :
        flag(false),
        mPlane(plane)
    {
    }

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection) override;
    virtual bool inside (Vector3dd &point)  override;
};


class RaytraceableTriangle : public Raytraceable {
public:
    static const double EPSILON;

    Triangle3dd mTriangle;

    RaytraceableTriangle(const Triangle3dd &triangle) :
        mTriangle(triangle)
    {
    }

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection) override;
    virtual bool inside (Vector3dd &point)  override;
};


class RaytraceableMesh : public Raytraceable {
public:
    static const double EPSILON;

    Mesh3DDecorated *mMesh;

    RaytraceableMesh(Mesh3DDecorated *mesh);

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection) override;
    virtual bool inside (Vector3dd &point)  override;
};

class NumPlaneFrame : public PlaneFrame {
public:
    int num;

    NumPlaneFrame(const PlaneFrame &frame, int num) :
        PlaneFrame(frame),
        num(num)
    {}


    bool intersect(RayIntersection &intersection)
    {
        double t;
        double u, v;
        if (!intersectWithOrtTriangle(intersection.ray, t, u, v))
            return false;

        if (t > 0.000001) {
            intersection.t = t;
            intersection.normal = getNormal();
            intersection.uvCoord = Vector2dd(u, v);
            intersection.payload = num;
            return true;
        }
        return false;
    }
};


class NumTriangle3dd : public Triangle3dd {
public:
    int num;

    NumTriangle3dd(const Triangle3dd &triangle, int num) :
        Triangle3dd(triangle),
        num(num)
    {}

    NumPlaneFrame toNumPlaneFrame() const
    {
        return NumPlaneFrame(toPlaneFrame(), num);
    }

    NumPlaneFrame toCache() const
    {
        return toNumPlaneFrame();
    }

    BSPSeparatorSide::BSPSeparatorSide side(const Plane3d &separator) const
    {
        bool b1 = (separator.pointWeight(p1()) > 0);
        bool b2 = (separator.pointWeight(p2()) > 0);
        bool b3 = (separator.pointWeight(p3()) > 0);

        if (b1 && b2 && b3) {
            return BSPSeparatorSide::LEFT_SIDE;
        }
        if (!b1 && !b2 && !b3) {
            return BSPSeparatorSide::RIGHT_SIDE;
        }
        return BSPSeparatorSide::MIDDLE;
    }

    void addToApproximation(EllipticalApproximation3d &v) const
    {
        for(int i = 0; i < SIZE; i++)
            v.addPoint(p[i]);
    }

    AxisAlignedBox3d getBoundingBox() const
    {
        Vector3dd minP = Vector3dd(numeric_limits<double>::max());
        Vector3dd maxP = Vector3dd(numeric_limits<double>::lowest());
        for (int i = 0; i < Triangle3dd::SIZE; i++)
        {
            for (int j = 0; j < Vector3dd::LENGTH; j++)
            {
                if (minP[j] > p[i][j]) minP[j] = p[i][j];
                if (maxP[j] < p[i][j]) maxP[j] = p[i][j];
            }
        }
        return AxisAlignedBox3d(minP, maxP);
    }

};



class RaytraceableOptiMesh : public RaytraceableMesh {
public:
    static const double EPSILON;
    typedef BSPTreeNodeRender<NumTriangle3dd, NumPlaneFrame> RaytraceableMeshTree;

    RaytraceableMeshTree *opt = NULL;

    RaytraceableOptiMesh(Mesh3DDecorated *mesh) :
        RaytraceableMesh(mesh)
    {}

    void optimize();
    void dumpToMesh(Mesh3D &mesh, bool plane, bool volume);

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection) override;

    virtual ~RaytraceableOptiMesh()
    {
        delete_safe(opt);
    }
};


class RaytraceableUnion : public Raytraceable {
public:
    vector<Raytraceable *> elements;

    typedef BSPTreeNodeRender<RaytraceableNodeWrapper, RaytraceableNodeWrapper> RaytraceableSubTree;
    RaytraceableSubTree *opt = NULL;
    void optimize();

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(RayIntersection &intersection) override;
    virtual bool inside (Vector3dd &point)  override;

    virtual bool toMesh (Mesh3D & target);
};


#endif // RAYTRACEOBJECTS_H
