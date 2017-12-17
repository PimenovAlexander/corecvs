#include "../math/vector/vector3d.h"
#include<set>

using namespace corecvs;
using namespace std;

class MyVector : public Vector3dd
{
public:
    MyVector(MyVector &V, MyVector &U) : Vector3dd(U.x() - V.x(), U.y() - V.y(), U.z() - V.z()) {}

    MyVector(const double x, const double y, const double z) : Vector3dd(x, y , z) {}

    MyVector(const MyVector &U) {
        (*this)[0] = U.x();
        (*this)[1] = U.y();
        (*this)[2] = U.z();
    }
    MyVector()
    {
        (*this)[0] = .0;
        (*this)[1] = .0;
        (*this)[2] = .0;
    }

    double dotProduct(const MyVector &U) {
        double ans = this->x() * U.x();
        ans += this->y() * U.y();
        ans += this->z() * U.z();
        return ans;
    }

    MyVector operator ^(const MyVector &U)
    {
        double x, y, z;
        x = this->y() * U.z() - U.y() * this->z();
        y = U.x() * this->z() - U.z() * this->x();
        z = U.y() * this->x() - U.x() * this->y();
        return MyVector(x, y, z);
    }


    MyVector& operator=(const MyVector &U) {
        (*this)[0] = U.x();
        (*this)[1] = U.y();
        (*this)[2] = U.z();
        return (*this);
    }


    bool operator ==(const MyVector &V) const{
        return (this->x() == V.x()) && (this->y() == V.y()) && (this->z() == V.z());
    }


    bool operator <(const MyVector &V) const{
        if (this->x() < V.x()) {
            return true;
        }
        if ((this->x() == V.x()) && (this->y() < V.y())) {
            return true;
        }
        if ((this->x() == V.x()) && (this->x() == V.x()) && (this->z() < V.z())) {
            return true;
        }
        return false;
    }

    double len2() {
        return this->x() * this->x() + this->y() * this->y() + this->z() * this->z();
    }
};

class Face {
private:
    vector<MyVector> points;
public:
    Face(MyVector &A, MyVector B, MyVector C) {
        points.push_back(A);
        points.push_back(B);
        points.push_back(C);
    }
    MyVector A() const {
        return points[0];
    }
    MyVector B() const {
        return points[1];
    }
    MyVector C() const {
        return points[2];
    }
    bool operator <(const Face &right) const{
        vector<MyVector> left_a = {this->A(), this->B(), this->C()};
        vector<MyVector> right_a = {right.A(), right.B(), right.C()};
        sort(left_a.begin(), left_a.end());
        sort(right_a.begin(), right_a.end());
        if (left_a[0] < right_a[0]) {
            return true;
        }
        if ((left_a[0] == right_a[0]) && (left_a[1] < right_a[1])) {
            return true;
        }
        if ((left_a[0] == right_a[0]) && (left_a[1] == right_a[1]) && (left_a[2] < right_a[2])) {
            return true;
        }
        return false;
    }
    bool operator ==(const Face &right) const{
        return (this->A() == right.A()) && (this->B() == right.B()) && (this->C() == right.C());
    }
    bool isUnder(MyVector &U, MyVector &center) {
        MyVector pivot1(points[0], points[1]);
        MyVector pivot2(points[0], points[2]);
        MyVector pivot3(points[0], U);
        MyVector pivot4(points[0], center);
        if (pivot1.dotProduct(pivot2 ^ pivot3) * pivot1.dotProduct(pivot2 ^ pivot4) < 0) { //right
            return true;
        }
        return false;
    }
    void print() {
        for (auto point : points) {
            cout << point.x() << " " << point.y() << " " << point.z() << endl;
        }
    }
};

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
    ConvexHull3D(vector<MyVector> points, double eps) {
        MyVector pivot1(points[0], points[1]);
        MyVector pivot2;
        MyVector pivot3;
        vector<MyVector> simplex;
        simplex.push_back(points[0]);
        simplex.push_back(points[1]);
        int idx = 2;
        while (idx < points.size()) {
            MyVector temp(points[0], points[idx]);
            idx++;
            double product = pivot1.dotProduct(temp);
            product *= product;
            if (abs(product - pivot1.len2() * temp.len2()) > eps) {
                pivot2 = temp;
                simplex.push_back(points[idx - 1]);
                swap(points[2], points[idx - 1]);
                break;
            }
        }
        while (idx < points.size()) {
            MyVector temp(points[0], points[idx]);
            idx++;
            if (abs(pivot1.dotProduct(temp ^ pivot2)) > eps) {
                pivot3 = temp;
                simplex.push_back(points[idx - 1]);
                swap(points[3], points[idx - 1]);
                break;
            }
        }
        double cX = .0;
        double cY = .0;
        double cZ = .0;
        for (int i = 0; i < simplex.size(); ++i) {
            cX += simplex[i].x();
            cY += simplex[i].y();
            cZ += simplex[i].z();
        }
        center = MyVector(cX / 4., cY / 4., cZ / 4.);
        faces.insert(Face(simplex[0], simplex[1], simplex[2]));
        faces.insert(Face(simplex[0], simplex[1], simplex[3]));
        faces.insert(Face(simplex[0], simplex[2], simplex[3]));
        faces.insert(Face(simplex[1], simplex[2], simplex[3]));
        for (int i = 4; i < points.size(); ++i) {
            set <pair<MyVector, MyVector>> visible;
            set <pair<MyVector, MyVector>> inVisible;
            vector<Face> toDel;
            for (auto face : faces) {
                if (face.isUnder(points[i], center)) {
                    visible.insert({face.A(), face.B()});
                    visible.insert({face.A(), face.C()});
                    visible.insert({face.C(), face.B()});
                    toDel.push_back(face);
                } else {
                    inVisible.insert({face.A(), face.B()});
                    inVisible.insert({face.A(), face.C()});
                    inVisible.insert({face.C(), face.B()});
                }
            }
            for (int i = 0; i < toDel.size(); ++i) {
                faces.erase(toDel[i]);
            }
            for (auto edge : inVisible) {
                if (visible.find(edge) != visible.end()) {
                    faces.insert(Face(points[i], edge.first, edge.second));
                    continue;
                }
                if (visible.find({edge.second, edge.first}) != visible.end()) {
                    faces.insert(Face(points[i], edge.first, edge.second));
                    continue;
                }
            }

        }
    }

    void print() {
        for(auto face : faces) {
            face.print();
            cout << endl;
        }
    }
};

class ConvexHullCalc {
private:
    vector<MyVector> points;
    double eps;
    ConvexHull *convexHull;

    bool equals(const MyVector &U, MyVector &V) {
        return (abs(V.x() - U.x()) < eps) && (abs(V.y() - U.y()) < eps) && (abs(V.z() - U.z()) < eps);
    }

    void deleteDuplicates() {
        int size = points.size();
        for (int i = 0; i < size; ++i) {
            for (int j = i + 1; j < size; ++j) {
                if (equals(points[i], points[j])) {
                    points[i].print();
                    points[j].print();
                    points.erase(points.begin() + i);
                    size--;
                    i--;
                    break;
                }
            }
        }
    }

    bool isTheSameLine() {
        MyVector pivot(points[0], points[1]);
        for (int i = 2; i < points.size(); ++i) {
            MyVector temp(points[0], points[i]);
            double product = pivot.dotProduct(temp);
            product *= product;
            if (abs(product - pivot.len2() * temp.len2()) > eps) {
                return false;
            }
        }
        return true;
    }

    bool isTheSamePlane() {
        MyVector pivot1(points[0], points[1]);
        MyVector pivot2(.0, .0, .0);
        int idx = 2;
        while (idx < points.size()) {
            MyVector temp(points[0], points[idx]);
            idx++;
            double product = pivot1.dotProduct(temp);
            product *= product;
            if (abs(product - pivot1.len2() * temp.len2()) > eps) {
                pivot2 = temp;
                break;
            }
        }
        for (; idx < points.size(); ++idx) {
            MyVector temp(points[0], points[idx]);
            if (abs(pivot1.dotProduct(temp ^ pivot2)) > eps) {
                return false;
            }
        }
        return true;
    }

public:
    ConvexHullCalc(vector<MyVector> pointsIn) {
        this->points = pointsIn;
        this->eps = 1e-9;
        this->convexHull = nullptr;
    }

    ConvexHullCalc(vector<MyVector> pointsIn, double epsIn) {
        if (epsIn < 0) {
            //throw new BadEps();
        }
        this->points = pointsIn;
        this->eps = epsIn;
        this->convexHull = nullptr;
    }

    void calc() {
        deleteDuplicates();
        if (points.size() < 3) {
            cout << "no points";
            return; // it is point or segment
        }

        if (isTheSameLine()) {
            cout << "line";
            return; // ll point on the same line
        }

        if (isTheSamePlane()) {
            cout << "2d" << endl;
            convexHull = new ConvexHull2D(points);
            return;
        }
        cout << "3d" << endl;
        convexHull = new ConvexHull3D(points, eps);
    }

    ConvexHull *getHull() {
        return convexHull;
    }
};

