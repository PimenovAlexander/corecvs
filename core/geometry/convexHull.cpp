#include "core/geometry/convexHull.h"

using namespace corecvs;
using namespace std;

// https://cw.fel.cvut.cz/wiki/_media/misc/projects/oppa_oi_english/courses/ae4m39vg/lectures/05-convexhull-3d.pdf


MyVector::MyVector(const Vector3dd &V, const Vector3dd &U) : Vector3dd(U - V) {}

MyVector::MyVector(const double x, const double y, const double z) : Vector3dd(x, y , z) {}

MyVector::MyVector(const Vector3dd &U) : Vector3dd(U)
{}

MyVector::MyVector()
{
    (*this)[0] = .0;
    (*this)[1] = .0;
    (*this)[2] = .0;
}

MyVector& MyVector::operator=(const MyVector &U) {
    (*this)[0] = U.x();
    (*this)[1] = U.y();
    (*this)[2] = U.z();
    return (*this);
}


bool MyVector::operator ==(const MyVector &V) const{
    return (this->x() == V.x()) && (this->y() == V.y()) && (this->z() == V.z());
}


bool MyVector::operator <(const MyVector &V) const{
    if (this->x() < V.x()) {
        return true;
    }
    if ((this->x() == V.x()) && (this->y() < V.y())) {
        return true;
    }
    if ((this->x() == V.x()) && (this->y() == V.y()) && (this->z() < V.z())) {
        return true;
    }
    return false;
}

Face::Face(const MyVector &A, const MyVector &B, const MyVector &C) :
    Triangle3dd(A, B, C)
{
}

bool Face::operator <(const Face &right) const{
    vector<MyVector> left_a  = {this->p1(), this->p2(), this->p3()};
    vector<MyVector> right_a = {right.p1(), right.p2(), right.p3()};

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

bool Face::operator ==(const Face &right) const{
    return (this->p1() == right.p1()) && (this->p2() == right.p2()) && (this->p3() == right.p3());
}

bool Face::isUnder(const Vector3dd &U, const Vector3dd &center) {
    MyVector pivot1(p[0], p[1]);
    MyVector pivot2(p[0], p[2]);
    MyVector pivot3(p[0], U);
    MyVector pivot4(p[0], center);
    if ((pivot3 & (pivot2 ^ pivot1)) * (pivot4 & (pivot2 ^ pivot1)) < 0) { //right
        return true;
    }
    return false;
}
void Face::print() {
    for (auto point : p) {
        cout << point.x() << " " << point.y() << " " << point.z() << endl;
    }
}

/*
ConvexHull3D. Used incremetal algorithm
*/

ConvexHull3D::ConvexHull3D(vector<Vector3dd> &points, double eps) {
    MyVector pivot1(points[0], points[1]);
    MyVector pivot2;
    MyVector pivot3;
    vector<MyVector> simplex;
    simplex.push_back(points[0]);
    simplex.push_back(points[1]);
    size_t idx = 2;
    while (idx < points.size()) {
        MyVector temp(points[0], points[idx]);
        idx++;
        double product = pivot1 & temp;
        product *= product;
        if (abs(product - pivot1.l2Metric() * temp.l2Metric()) > eps) {
            pivot2 = temp;
            simplex.push_back(points[idx - 1]);
            swap(points[2], points[idx - 1]);
            break;
        }
    }
    while (idx < points.size()) {
        MyVector temp(points[0], points[idx]);
        idx++;
        if (abs(pivot1 & (temp ^ pivot2)) > eps) {
            pivot3 = temp;
            simplex.push_back(points[idx - 1]);
            swap(points[3], points[idx - 1]);
            break;
        }
    }
    double cX = .0;
    double cY = .0;
    double cZ = .0;
    for (size_t i = 0; i < simplex.size(); ++i) {
        cX += simplex[i].x();
        cY += simplex[i].y();
        cZ += simplex[i].z();
    }
    center = MyVector(cX / 4., cY / 4., cZ / 4.);
    faces.insert(Face(simplex[0], simplex[1], simplex[2]));
    faces.insert(Face(simplex[0], simplex[1], simplex[3]));
    faces.insert(Face(simplex[0], simplex[2], simplex[3]));
    faces.insert(Face(simplex[1], simplex[2], simplex[3]));
    for (size_t i = 4; i < points.size(); ++i) {
        set <pair<MyVector, MyVector>> visible;
        set <pair<MyVector, MyVector>> inVisible;
        vector<Face> toDel;
        for (auto face : faces) {
            if (face.isUnder(points[i], center)) {
                visible.insert({face.p1(), face.p2()});
                visible.insert({face.p1(), face.p3()});
                visible.insert({face.p3(), face.p2()});
                toDel.push_back(face);
            } else {
                inVisible.insert({face.p1(), face.p2()});
                inVisible.insert({face.p1(), face.p3()});
                inVisible.insert({face.p3(), face.p2()});
            }
        }
        for (size_t i = 0; i < toDel.size(); ++i) {
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

void ConvexHull3D::print() {
    for(auto face : faces) {
        face.print();
        cout << endl;
    }
}

bool ConvexHullCalc::equals(const Vector3dd &U, Vector3dd &V) {
    return (abs(V.x() - U.x()) < eps) && (abs(V.y() - U.y()) < eps) && (abs(V.z() - U.z()) < eps);
}

void ConvexHullCalc::deleteDuplicates() {
    int size = (int)points.size();
    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            if (equals(points[i], points[j])) {
                points.erase(points.begin() + i);
                size--;
                i--;
                break;
            }
        }
    }
}

bool ConvexHullCalc::isTheSameLine() {
    MyVector pivot(points[0], points[1]);
    for (size_t i = 2; i < points.size(); ++i) {
        MyVector temp(points[0], points[i]);
        double product = pivot & temp;
        product *= product;
        if (abs(product - pivot.l2Metric() * temp.l2Metric()) > eps) {
            return false;
        }
    }
    return true;
}

bool ConvexHullCalc::isTheSamePlane() {
    MyVector pivot1(points[0], points[1]);
    MyVector pivot2(.0, .0, .0);
    size_t idx = 2;
    while (idx < points.size()) {
        MyVector temp(points[0], points[idx]);
        idx++;
        double product = pivot1 & temp;
        product *= product;
        if (abs(product - pivot1.l2Metric() * temp.l2Metric()) > eps) {
            pivot2 = temp;
            break;
        }
    }
    for (; idx < points.size(); ++idx) {
        MyVector temp(points[0], points[idx]);
        if (abs(pivot1 & (temp ^ pivot2)) > eps) {
            return false;
        }
    }
    return true;
}

ConvexHullCalc::ConvexHullCalc(const vector<Vector3dd> &pointsIn, double epsIn) :
    points(pointsIn),
    eps(epsIn),
    convexHull(NULL)
{}

void ConvexHullCalc::calc() {
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

ConvexHullResult* ConvexHullCalc::getHull() {
    return convexHull;
}

