#include "../math/vector/vector3d.h"
#include<set>

using namespace corecvs;
using namespace std;

class MyVector : public Vector3dd
{
public:
    MyVector(MyVector &V, MyVector &U);
    MyVector(const double x, const double y, const double z);
    MyVector(const MyVector &U);
    MyVector();

    double dotProduct(const MyVector &U);

    MyVector operator ^(const MyVector &U);

    MyVector& operator=(const MyVector &U);

    bool operator ==(const MyVector &V) const;


    bool operator <(const MyVector &V) const;

    double len2();
};

class Face {
private:
    vector<MyVector> points;
public:
    Face(MyVector &A, MyVector B, MyVector C);
    MyVector A() const;
    MyVector B() const;
    MyVector C() const;
    bool operator <(const Face &right) const;
    bool operator ==(const Face &right) const;
    bool isUnder(MyVector &U, MyVector &center);
    void print();
};
/*
ConvexHull an abstract class for convex hull
*/
class ConvexHull {
public:
    virtual void print() = 0;
};

class ConvexHull2D : public ConvexHull {
public:
    ConvexHull2D(vector<MyVector> points) {}
    void print() {}
};


/*
ConvexHull3D. Used incremetal algorithm
*/
class ConvexHull3D : public ConvexHull {
private:
    MyVector center;
    set <Face> faces;
public:
    ConvexHull3D(vector<MyVector> points, double eps);
    void print();
};

/*
ConvexHullCalc. Calculate convex hull.
It takes vector of points and epsilon(optional) as constructor parameters.
To caculate convex hull use calc()
To get convex hull object use getHull()
example:
    MyVector a1(0, 0, 0);
    MyVector a2(5, 0, 0);
    MyVector a3(0, 5, 0);
    MyVector a4(0, 0, 5);
    MyVector a5(5, 5, 5);
    MyVector a6(5, 5, 0);
    MyVector a7(5, 0, 5);
    MyVector a8(0, 5, 5);
    vector<MyVector> temp = {a1, a2, a3, a4, a5, a6, a7, a8};
    ConvexHullCalc temp(temp, 1e-20);
    temp.calc();
    ConvexHull *hull = temp.getHull(); --get convex hull
    hull->print(); --print convex hull (set of faces)
*/
class ConvexHullCalc {
private:
    vector<MyVector> points;
    double eps;
    ConvexHull *convexHull;

    bool equals(const MyVector &U, MyVector &V);

    void deleteDuplicates();
    bool isTheSameLine();

    bool isTheSamePlane();

public:
    ConvexHullCalc(vector<MyVector> pointsIn);

    ConvexHullCalc(vector<MyVector> pointsIn, double epsIn);

    void calc();

    ConvexHull *getHull();
};

