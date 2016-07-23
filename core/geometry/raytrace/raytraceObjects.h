#ifndef RAYTRACEOBJECTS_H
#define RAYTRACEOBJECTS_H

#include "raytrace/raytraceRenderer.h"

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
    virtual void normal(const Vector3dd &vector, Vector3dd &normal)   override;
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
    virtual void normal(const Vector3dd &vector, Vector3dd &normal)   override;
    virtual bool inside (Vector3dd &point)  override;
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
    virtual void normal(const Vector3dd &vector, Vector3dd &normal) override;
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
    virtual void normal(const Vector3dd &vector, Vector3dd &normal) override;
    virtual bool inside (Vector3dd &point)  override;
};


class RaytraceableMesh : public Raytraceable {
public:
    static const double EPSILON;

    Mesh3D *mMesh;

    RaytraceableMesh(Mesh3D *mesh);

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(const Vector3dd &vector, Vector3dd &normal) override;
    virtual bool inside (Vector3dd &point)  override;
};

class RaytraceableOptiMesh : public RaytraceableMesh {
public:
    static const double EPSILON;

    struct TreeNode {
        vector<Triangle3dd> submesh;
        vector<PlaneFrame>  cached;


        TreeNode *middle = NULL;
        TreeNode *left = NULL;
        TreeNode *right = NULL;

        Sphere3d bound;
        AxisAlignedBox3d box;
        Plane3d  plane;

        bool intersect(RayIntersection &intersection);
        void subdivide();
        void cache();

        int childCount();
        int triangleCount();


        void dumpToMesh(Mesh3D &mesh, int depth, bool plane, bool volume);

        ~TreeNode()
        {
            delete_safe(left);
            delete_safe(right);
        }
    };

    TreeNode *opt = NULL;

    RaytraceableOptiMesh(Mesh3D *mesh) :
        RaytraceableMesh(mesh)
    {}

    void optimize();
    void dumpToMesh(Mesh3D &mesh, bool plane, bool volume);

    virtual bool intersect(RayIntersection &intersection) override;

    virtual ~RaytraceableOptiMesh()
    {
        delete_safe(opt);
    }
};


class RaytraceableUnion : public Raytraceable {
public:
    vector<Raytraceable *> elements;

    virtual bool intersect(RayIntersection &intersection) override;
    virtual void normal(const Vector3dd &vector, Vector3dd &normal) override;
    virtual bool inside (Vector3dd &point)  override;
};


#endif // RAYTRACEOBJECTS_H
