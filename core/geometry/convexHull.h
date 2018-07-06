#include <set>

#include "core/math/vector/vector3d.h"
#include "core/geometry/polygons.h"


using namespace corecvs;


class MyVector : public Vector3dd
{
public:
    MyVector(const Vector3dd &V, const Vector3dd &U);
    MyVector(const double x, const double y, const double z);
    MyVector(const Vector3dd &U);
    MyVector();

    MyVector& operator=(const MyVector &U);

    bool operator ==(const MyVector &V) const;

    bool operator <(const MyVector &V) const;
};

class Face : public Triangle3dd {
public:
    Face(const MyVector &A, const MyVector &B, const MyVector &C);
    bool operator <(const Face &right) const;
    bool operator ==(const Face &right) const;
    bool isUnder(const Vector3dd &U, const Vector3dd &center);
    void print();
};
/*
ConvexHull an abstract class for convex hull
*/
class ConvexHullResult {
public:
    virtual bool is3D() = 0;
    virtual void print() = 0;
};

class ConvexHull2D : public ConvexHullResult {
public:
    ConvexHull2D(const vector<Vector3dd> &/*points*/) {}
    virtual bool is3D() override { return false; }
    virtual void print() override {}
};


/*
ConvexHull3D. Used incremetal algorithm
*/
class ConvexHull3D : public ConvexHullResult {
private:
    MyVector center;

public:
    std::set <Face> faces;
public:
    ConvexHull3D(vector<Vector3dd> &points, double eps);
    virtual bool is3D() override { return true; }
    virtual void print() override;
};

/**
    ConvexHullCalc. Calculate convex hull.
    It takes vector of points and epsilon(optional) as constructor parameters.
    To caculate convex hull use calc()
    To get convex hull object use getHull()
    example:
        Vector3dd a1(0, 0, 0);
        Vector3dd a2(5, 0, 0);
        Vector3dd a3(0, 5, 0);
        Vector3dd a4(0, 0, 5);
        Vector3dd a5(5, 5, 5);
        Vector3dd a6(5, 5, 0);
        Vector3dd a7(5, 0, 5);
        Vector3dd a8(0, 5, 5);
        vector<MyVector> temp = {a1, a2, a3, a4, a5, a6, a7, a8};
        ConvexHullCalc temp(temp, 1e-20);
        temp.calc();
        ConvexHull *hull = temp.getHull(); --get convex hull
        hull->print(); --print convex hull (set of faces)
**/
class ConvexHullCalc {
private:
    vector<Vector3dd> points;
    double eps;
    ConvexHullResult *convexHull = NULL;

    bool equals(const Vector3dd &U, Vector3dd &V);

    void deleteDuplicates();
    bool isTheSameLine();

    bool isTheSamePlane();

public:
    ConvexHullCalc(const vector<Vector3dd> &pointsIn, double epsIn=1e-9);

    void calc();

    ConvexHullResult *getHull();
};

