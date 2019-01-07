#include "projectiveConvexQuickHull.h"
#include "mesh3d.h"

using namespace std;

namespace corecvs {

double pointLineDist(const ProjectiveCoord4d &lineP1, const ProjectiveCoord4d &lineP2, const ProjectiveCoord4d &point) {
    if (lineP1 == point || lineP2 == point)
        return 0;
    if (lineP1.w() == 0 || lineP2.w() == 0 || point.w() == 0)
        return std::numeric_limits<double>::infinity();
    ProjectiveCoord4d lineVect = lineP2 - lineP1;
    lineVect = lineVect + ProjectiveCoord4d(0, 0, 0, 1);
    return (lineVect.toVector() ^ (point.toVector() - lineP1.toVector())).l2Metric() / lineVect.xyz().l2Metric();
}

double pointPlaneDist(const ProjectiveCoord4d &planeP1, const ProjectiveCoord4d &planeP2,
                      const ProjectiveCoord4d &planeP3, const ProjectiveCoord4d &point) {
    if (planeP1 == point || planeP2 == point || planeP3 == point)
        return 0;
    if (planeP1.w() == 0 || planeP2.w() == 0 || planeP3.w() == 0 || point.w() == 0)
        return std::numeric_limits<double>::infinity();
    auto baseV1 = planeP2.toVector() - planeP1.toVector();
    auto baseV2 = planeP3.toVector() - planeP1.toVector();
    auto prod = baseV1 & (baseV2 ^ (point.toVector() - planeP1.toVector()));
    return prod / ((baseV1 ^ baseV2).l2Metric());
}

double pointFaceDist(const Triangle4dd &face, const ProjectiveCoord4d &point) {
    return pointPlaneDist(face.p1(), face.p2(), face.p3(), point);
}

bool faceIsVisible(const ProjectiveCoord4d &eyePoint, const ProjectiveConvexQuickHull::HullFace &face, double eps) {
    if (eyePoint == face.plane.p1() || eyePoint == face.plane.p2() || eyePoint == face.plane.p3())
        return false;
    // one line
    if (face.plane.p1() == face.plane.p2() || face.plane.p1() == face.plane.p3() || face.plane.p1() == face.plane.p3())
        return false;

    // 3 points on infinity
    if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0 && face.plane.p3().w() == 0) {
        Vector3dd p = (face.plane.p1() - face.plane.p2()).xyz();
        Vector3dd q = (face.plane.p1() - face.plane.p3()).xyz();
        auto normal = p ^ q;
        auto res = -1 * face.plane.p1().xyz() & normal.normalised();
        return res >= eps;
    }
    Vector3dd normal = face.plane.getNormal();
    Vector3dd point = face.plane.p1().toVector();

    // 2 points on infinity
    if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0) {
        normal = face.plane.p1().xyz() ^ face.plane.p2().xyz();
        point = face.plane.p3().toVector();
    } else if (face.plane.p1().w() == 0 && face.plane.p3().w() == 0) {
        normal = face.plane.p3().xyz() ^ face.plane.p1().xyz();
        point = face.plane.p2().toVector();
    } else if (face.plane.p2().w() == 0 && face.plane.p3().w() == 0) {
        normal = face.plane.p2().xyz() ^ face.plane.p3().xyz();
    }
    // 1 point on infinity
    else if (face.plane.p1().w() == 0) {
        Vector3dd q = face.plane.p1().xyz();
        ProjectiveCoord4d p = face.plane.p2() - face.plane.p3();
        normal = q ^ p.toVector();
        point = face.plane.p2().toVector();
    } else if (face.plane.p2().w() == 0) {
        Vector3dd q = face.plane.p2().xyz();
        ProjectiveCoord4d p = face.plane.p1() - face.plane.p3();
        normal = q ^ p.toVector();
    } else if (face.plane.p3().w() == 0) {
        Vector3dd q = face.plane.p3().xyz();
        ProjectiveCoord4d p = face.plane.p1() - face.plane.p2();
        normal = p.toVector() ^ q;
    }

    double res;
    if (eyePoint.w() == 0)
        res = eyePoint.xyz() & normal.normalised();
    else
        res = ((eyePoint.toVector() - point) & normal.normalised());
    return res >= eps;
}

void addPointsToFaces(ProjectiveConvexQuickHull::HullFace *faces, unsigned long faces_count,
        const ProjectiveConvexQuickHull::Vertices &listVertices, double eps) {
    for (const ProjectiveCoord4d &vertex : listVertices)
        for (unsigned long i = 0; i < faces_count; i++) {
            auto &face = faces[i];
            if (faceIsVisible(vertex, face, eps)) {
                faces[i].points.push_back(vertex);
                break;
            }
        }
}

ProjectiveConvexQuickHull::Vertices createSimplex(const ProjectiveConvexQuickHull::Vertices& listVertices) {
    ProjectiveCoord4d first = listVertices.front();
    ProjectiveCoord4d EP[6] = {first, first, first, first, first, first};

    for (const auto &vertex : listVertices) {
        if (vertex.X() <= EP[0].X()) EP[0] = vertex;
        if (vertex.X() >= EP[1].X()) EP[1] = vertex;
        if (vertex.Y() <= EP[2].Y()) EP[2] = vertex;
        if (vertex.Y() >= EP[3].Y()) EP[3] = vertex;
        if (vertex.Z() <= EP[4].Z()) EP[4] = vertex;
        if (vertex.Z() >= EP[5].Z()) EP[5] = vertex;
    }

    ProjectiveCoord4d triangleP1 = EP[0], triangleP2 = EP[0], triangleP3 = EP[0];
    double maxDist = -1;
    for (const ProjectiveCoord4d &point1 : EP)
        for (const ProjectiveCoord4d &point2 : EP) {
            double dist = point2.dist(point1);
            if (dist >= maxDist && point1 != point2) {
                maxDist = dist;
                triangleP1 = point1;
                triangleP2 = point2;
            }
        }

    maxDist = pointLineDist(triangleP1, triangleP2, triangleP3);
    for (const auto &point : EP) {
        double dist = pointLineDist(triangleP1, triangleP2, point);
        if (dist >= maxDist && point != triangleP1 && point != triangleP2) {
            maxDist = dist;
            triangleP3 = point;
        }
    }

    maxDist = 0;
    ProjectiveCoord4d apex = EP[0];
    for (const auto &point : listVertices) {
        double dist = abs(pointPlaneDist(triangleP1, triangleP2, triangleP3, point));
        if (dist == 0)
            continue;
        bool f = faceIsVisible(triangleP3, ProjectiveConvexQuickHull::HullFace(apex, triangleP1, triangleP2), 0);
        bool f1 = faceIsVisible(point, ProjectiveConvexQuickHull::HullFace(apex, triangleP1, triangleP2), 0);
        bool f2 = faceIsVisible(point, ProjectiveConvexQuickHull::HullFace(apex, triangleP2, triangleP1), 0);
        bool res = f ? f1 : f2;
        if (dist >= maxDist && point != triangleP1 && point != triangleP2 && point != triangleP3 && !res) {
            maxDist = dist;
            apex = point;
        }
    }
    ProjectiveConvexQuickHull::Vertices Res;
    if (faceIsVisible(apex, ProjectiveConvexQuickHull::HullFace(triangleP1, triangleP2, triangleP3), 0))
        Res = { triangleP1, triangleP3, triangleP2, apex };
    else Res = { triangleP1, triangleP2, triangleP3, apex };
    return Res;
}

ProjectiveConvexQuickHull::HullFaces ProjectiveConvexQuickHull::quickHull(const ProjectiveConvexQuickHull::Vertices &listVertices, double epsilon) {
    if (listVertices.size() < 4)
        return ProjectiveConvexQuickHull::HullFaces();

    Vertices simplex = createSimplex(listVertices);
    Vertices uniqueSimplex;

    for (ProjectiveCoord4d& elem : simplex)
        if (find(uniqueSimplex.begin(), uniqueSimplex.end(), elem) == uniqueSimplex.end())
            uniqueSimplex.push_back(elem);

    if (uniqueSimplex.size() == 1) {
        printf("Only one unique point\n");
        return {};
    }

    if (uniqueSimplex.size() == 2) {
        printf("This is line\n");
        return {};
    }

    if (uniqueSimplex.size() == 3) {
        printf("This is plane\n");
        return {};
    }

    HullFaces faces = {
            HullFace(simplex[0], simplex[1], simplex[2]),
            HullFace(simplex[0], simplex[2], simplex[3]),
            HullFace(simplex[1], simplex[3], simplex[2]),
            HullFace(simplex[0], simplex[3], simplex[1])};

    queue<HullFace> queue;
    addPointsToFaces(faces.data(), faces.size(), listVertices, epsilon);
    for (auto face : faces) {
        if (!face.points.empty())
            queue.push(face);
    }

    while (!queue.empty()) {
        HullFace face = queue.front();
        queue.pop();

        if (!face.points.empty()) {
            double maxDist = -1;
            ProjectiveCoord4d furthest = {0, 0, 0, 1};
            for (const auto &point : face.points) {
                double dist = pointFaceDist(face.plane, point);
                if (dist > maxDist) {
                    maxDist = dist;
                    furthest = point;
                }
            }

            Vertices listUnclaimedVertices;
            HullFaces newFaces;

            for (HullFace &curFace : faces)
                if (faceIsVisible(furthest, curFace, epsilon)) {
                    listUnclaimedVertices.insert(listUnclaimedVertices.end(), curFace.points.begin(), curFace.points.end());

                    HullFaces tmpFaces;
                    tmpFaces.reserve(3);
                    tmpFaces.emplace_back(curFace.plane.p3(), furthest, curFace.plane.p2());
                    tmpFaces.emplace_back(curFace.plane.p1(), curFace.plane.p2(), furthest);
                    tmpFaces.emplace_back(curFace.plane.p1(), furthest, curFace.plane.p3());


                    for (HullFace &tmpFace : tmpFaces) {
                        auto alreadyExists = find(newFaces.begin(), newFaces.end(), tmpFace);
                        if (alreadyExists != newFaces.end())
                            newFaces.erase(alreadyExists);
                        else
                            newFaces.push_back(tmpFace);
                    }
                    curFace.toDelete = true;
                }

            faces.erase(remove_if(
                    faces.begin(),
                    faces.end(),
                    [](HullFace &face) {return face.toDelete;}
            ), faces.end());

            addPointsToFaces(newFaces.data(), newFaces.size(), listUnclaimedVertices, epsilon);
            for (auto &newFace : newFaces) {
                faces.push_back(newFace);
                queue.push(newFace);
            }
        }
    }
    cout << "Result: \n";
    for (auto &face: faces)
        cout << face.plane.p1() << " " << face.plane.p2() << " " << face.plane.p3() << "\n";
    return faces;
}


ProjectiveConvexQuickHull::HullFaces ProjectiveConvexQuickHull::intersect(const ProjectiveConvexQuickHull::HullFaces &v1, const ProjectiveConvexQuickHull::HullFaces &v2)
{
    vector<ProjectiveCoord4d> intersection;
    
    cout << "\nFind points of poly1 inside poly2\n";
    for (ProjectiveCoord4d &v : v1.getPoints())
    {
        if (v2.isInside(v)) 
        {
            cout << "Point: " << v << " is inside poly2\n";
            intersection.push_back(v);
        }
    }
    
    cout << "\nFind points of poly2 inside poly1\n";
    for (ProjectiveCoord4d &v : v2.getPoints())
    {
        if (v1.isInside(v)) 
        {
            cout << "Point: " << v << " is inside poly1\n";
            intersection.push_back(v);
        }
    }
    cout << "Points in intersection: " << intersection.size() << "\n";

    cout << "\nFind rays of poly1, that intersect poly2\n";
    for (const ProjectiveConvexQuickHull::HullFace &f : v1)
    {
        ProjectiveCoord4d p1 = f.plane.p[0];
        ProjectiveCoord4d p2 = f.plane.p[1];
        ProjectiveCoord4d p3 = f.plane.p[2];

        for (const ProjectiveConvexQuickHull::HullFace &f1 : v2)
        {
            ProjectiveCoord4d pointIntersection1;
            ProjectiveCoord4d pointIntersection2;
            //1
            int flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p1, p2, f1.plane, pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if(flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
            //2
            flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p2, p3, f1.plane, pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if( flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
            //3
            flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p3, p1, f1.plane, pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if( flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
        }
    }
    cout << "Points in intersection: " << intersection.size() << "\n";

    cout << "\nFind rays of poly2, that intersect poly1\n";
    for (const ProjectiveConvexQuickHull::HullFace &f : v2)
    {
        ProjectiveCoord4d p1 = f.plane.p[0];
        ProjectiveCoord4d p2 = f.plane.p[1];
        ProjectiveCoord4d p3 = f.plane.p[2];

        for (const ProjectiveConvexQuickHull::HullFace &f1 : v1)
        {
            ProjectiveCoord4d pointIntersection1;
            ProjectiveCoord4d pointIntersection2;
            //1
            int flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p1, p2, f1.plane,pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if( flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
            //2
            flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p2, p3, f1.plane,pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if( flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
            //3
            flag = ProjectiveConvexQuickHull::intersectSegmentTriangle4d(p3, p1, f1.plane,pointIntersection1, pointIntersection2);
            if(flag == 2)
            {
                intersection.push_back(pointIntersection1);
                intersection.push_back(pointIntersection2);
            }
            else if( flag == 1) 
            {
                intersection.push_back(pointIntersection1);
            }
        }
    }
    cout << "Points in intersection: " << intersection.size() << "\n";
    return ProjectiveConvexQuickHull::quickHull(intersection);
}

vector<ProjectiveCoord4d> ProjectiveConvexQuickHull::HullFaces::getPoints(bool dedup) const
{
    vector<ProjectiveCoord4d> toReturn;
    toReturn.reserve(size() * 4);
    for (const ProjectiveConvexQuickHull::HullFace &f : *this )
    {
        toReturn.push_back(f.plane.p1());
        toReturn.push_back(f.plane.p2());
        toReturn.push_back(f.plane.p3());
    }
    if (dedup) {
        std::sort(toReturn.begin(), toReturn.end(), [](ProjectiveCoord4d &a, ProjectiveCoord4d &b)
            {
                if (a.X() < b.X()) return true;
                if (a.X() > b.X()) return false;

                if (a.Y() < b.Y()) return true;
                if (a.Y() > b.Y()) return false;

                if (a.Z() < b.Z()) return true;
                return false;
            }
        );
        toReturn.erase(std::unique(toReturn.begin(), toReturn.end()), toReturn.end());//норм ли?
    }
    return toReturn;
}

bool ProjectiveConvexQuickHull::HullFaces::isInside(const ProjectiveCoord4d &p) const
{
    for (const HullFace &face: *this)
    {
        //point is NOT on infinity
        if(p.w() != 0)
        {
            // 3 points on infinity
            if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0 && face.plane.p3().w() == 0) continue;
            
            // 0 points on infinity
            Vector3dd normal = face.plane.getNormal();
            Vector3dd point = face.plane.p1().toVector();

            // 2 points on infinity
            if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0) 
            {
                normal = face.plane.p1().xyz() ^ face.plane.p2().xyz();
                point = face.plane.p3().toVector();
            } 
            else if (face.plane.p1().w() == 0 && face.plane.p3().w() == 0) 
            {
                normal = face.plane.p3().xyz() ^ face.plane.p1().xyz();
                point = face.plane.p2().toVector();
            } 
            else if (face.plane.p2().w() == 0 && face.plane.p3().w() == 0) 
            {
                normal = face.plane.p2().xyz() ^ face.plane.p3().xyz();
                point = face.plane.p1().toVector();
            }
            // 1 point on infinity
            else if (face.plane.p1().w() == 0) 
            {
                Vector3dd q = face.plane.p1().xyz();
                Vector3dd p = face.plane.p2().toVector() - face.plane.p3().toVector();
                normal = q ^ p;
                point = face.plane.p2().toVector();
            } 
            else if (face.plane.p2().w() == 0) 
            {
                Vector3dd q = face.plane.p2().xyz();
                Vector3dd p = face.plane.p1().toVector() - face.plane.p3().toVector();
                normal = q ^ p;
                point = face.plane.p3().toVector();
            } 
            else if (face.plane.p3().w() == 0) 
            {
                Vector3dd q = face.plane.p3().xyz();
                Vector3dd p = face.plane.p1().toVector() - face.plane.p2().toVector();
                normal = p ^ q;
                point = face.plane.p1().toVector();
            }
            double pointWeight = (normal & p.toVector()) - (normal & point);
            cout << "Face: " << face.plane << "point to check" << p << " FacePoint: " << point<<" pW" << pointWeight<< endl;
            if (pointWeight > 0)
            {
                return false;
            }
        }
        //point is on infinity
        else if (p.w() == 0)
        {
            // 3 points on infinity
            if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0 && face.plane.p3().w() == 0) return false;
            
            // 0 points on infinity
            Vector3dd normal = face.plane.getNormal();
            Vector3dd point = Vector3dd::Zero();

            // 2 points on infinity
            if (face.plane.p1().w() == 0 && face.plane.p2().w() == 0) 
            {
                normal = face.plane.p1().xyz() ^ face.plane.p2().xyz();
                point = Vector3dd::Zero();
            } 
            else if (face.plane.p1().w() == 0 && face.plane.p3().w() == 0) 
            {
                normal = face.plane.p3().xyz() ^ face.plane.p1().xyz();
                point = Vector3dd::Zero();
            } 
            else if (face.plane.p2().w() == 0 && face.plane.p3().w() == 0) 
            {
                normal = face.plane.p2().xyz() ^ face.plane.p3().xyz();
                point = Vector3dd::Zero();
            }
            // 1 point on infinity
            else if (face.plane.p1().w() == 0) 
            {
                Vector3dd q = face.plane.p1().xyz();
                Vector3dd p = face.plane.p2().toVector() - face.plane.p3().toVector();
                normal = q ^ p;
                point = Vector3dd::Zero();
            } 
            else if (face.plane.p2().w() == 0) 
            {
                Vector3dd q = face.plane.p2().xyz();
                Vector3dd p = face.plane.p1().toVector() - face.plane.p3().toVector();
                normal = q ^ p;
                point = Vector3dd::Zero();
            } 
            else if (face.plane.p3().w() == 0) 
            {
                Vector3dd q = face.plane.p3().xyz();
                Vector3dd p = face.plane.p1().toVector() - face.plane.p2().toVector();
                normal = p ^ q;
                point = Vector3dd::Zero();
            }
            double pointWeight = (normal & p.toVector()) - (normal & point);
            cout << "Face: " << face.plane << "point to check" << p << " FacePoint: " << point<<" pW" << pointWeight<< endl;
            if (pointWeight > 0)
            {
                return false;
            }
        }
    }
    return true;
}

// Assume that classes are already given for the objects:
//    Point and Vector with
//        coordinates {float x, y, z;}
//        operators for:
//            == to test  equality
//            != to test  inequality
//            (Vector)0 =  (0,0,0)         (null vector)
//            Point   = Point ± Vector
//            Vector =  Point - Point
//            Vector =  Scalar * Vector    (scalar product)
//            Vector =  Vector * Vector    (cross product)
//    Line and Ray and Segment with defining  points {Point P0, P1;}
//        (a Line is infinite, Rays and  Segments start at P0)
//        (a Ray extends beyond P1, but a  Segment ends at P1)
//    Plane with a point and a normal {Point V0; Vector  n;}
//    triangle with defining vertices {Point V0, V1, V2;}
//    Polyline and Polygon with n vertices {int n;  Point *V;}
//        (a Polygon has V[n]=V[0])
//===================================================================
 

#define SMALL_NUM   0.00000001 // anything that avoids division overflow
// dot product (3D) which allows vector operations in arguments
#define dot(u,v)   ((u).x() * (v).x() + (u).y() * (v).y() + (u).z() * (v).z())

int ProjectiveConvexQuickHull::intersectSegmentTriangle4d(ProjectiveCoord4d &start, ProjectiveCoord4d &end, const Triangle4dd &triangle, ProjectiveCoord4d &pointIntersection1,ProjectiveCoord4d &pointIntersection2 )
{
    //if 2 points of segment on infinity and points of trangle - not
    if(start.w() == 0 && end.w() == 0 && triangle.p1().w() != 0 && triangle.p2().w() != 0 && triangle.p3().w() != 0) return 0;
    //if 3 points of triangle on infinity
    if(triangle.p1().w() == 0 && triangle.p2().w() == 0 && triangle.p3().w() == 0)
    {
        //if 2 points of segment are NOT on infinity
        if(start.w() != 0 && end.w() != 0) return 0;
        //if 1 point of segment on infinity
        if(start.w() == 0 && end.w() != 0)
        {
            ProjectiveCoord4d temp = start;
            start = end;
            end = temp;
        }
        if(start.w() != 0 && end.w() == 0)
        {
            if(ProjectiveConvexQuickHull::isInsidePointTriangleInf(end,triangle)) 
            {
                pointIntersection1 = end;
                return 1;
            }
            if(ProjectiveConvexQuickHull::intersectSegmentPointInf(triangle.p1(), triangle.p2(), end))
            {
                pointIntersection1 = end;
                return 1;
            }
            if(ProjectiveConvexQuickHull::intersectSegmentPointInf(triangle.p2(), triangle.p3(), end))
            {
                pointIntersection1 = end;
                return 1;
            }
            if(ProjectiveConvexQuickHull::intersectSegmentPointInf(triangle.p3(), triangle.p1(), end))
            {
                pointIntersection1 = end;
                return 1;
            }
            return 0;
        }
    } 

    //if 3 points of triangle NOT on infinity (and 0 or 1 of segment on infinity)
    if(triangle.p1().w() != 0 && triangle.p2().w() != 0 && triangle.p3().w() != 0)
    {
        if(start.w() == 0)
        {
            ProjectiveCoord4d temp = start;
            start = end;
            end = temp;
        } 
        Ray3d ray;
        if(end.w() == 0) ray = Ray3d::FromOriginAndDirection(start.toVector(), end.xyz());
        else ray = Ray3d::FromPoints(start.toVector(), end.toVector());

        Vector3dd    u, v, n;              // triangle vectors
        Vector3dd    dir, w0, w, I;           // ray vectors
        float     r, a, b;              // params to calc ray-plane intersect

        // get triangle edge vectors and plane normal
        u = triangle.p2().toVector() - triangle.p1().toVector();
        v = triangle.p3().toVector() - triangle.p1().toVector();
        n = u ^ v;              // cross product
        if (n == Vector3dd(0,0,0))             // triangle is degenerate
            return -1;                  // do not deal with this case

        dir = ray.a;// ray direction vector
        w0 = ray.p - triangle.p1().toVector();              
        a = -dot(n,w0);
        b = dot(n,dir);
        if (fabs(b) < SMALL_NUM) {     // ray is  parallel to triangle plane
            if (a == 0)                 // ray lies in triangle plane
            {
                //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                return 2;
            }
               
            else return 0;              // ray disjoint from plane
        }

        // get intersect point of ray with triangle plane
        r = a / b;
        //if 1 point of segment on infinity => ray
        if (r < 0.0)                    // ray goes away from triangle
            return 0;                   // => no intersect
        // if a segment (0 points on infinity)
        if (end.w() != 0 && r > 1.0) return 0;

        I = ray.p + r * dir;            // intersect point of ray and plane
        pointIntersection1 = ProjectiveCoord4d(I,1);

        // is I inside triangle?
        float    uu, uv, vv, wu, wv, D;
        uu = dot(u,u);
        uv = dot(u,v);
        vv = dot(v,v);
        w = I - triangle.p1().toVector();
        wu = dot(w,u);
        wv = dot(w,v);
        D = uv * uv - uu * vv;

        // get and test parametric coords
        float s, t;
        s = (uv * wv - vv * wu) / D;
        if (s < 0.0 || s > 1.0)         // I is outside triangle
            return 0;
        t = (uv * wu - uu * wv) / D;
        if (t < 0.0 || (s + t) > 1.0)  // I is outside triangle
            return 0;
        return 1;                       // I is in triangle
    }

    //if 2 points of segment on infinity
    if(start.w() == 0 && end.w() == 0)
    {
        int flag;
        //3 point of triangle on infinity
        if(triangle.p1().w() == 0 && triangle.p2().w() == 0  && triangle.p3().w() == 0)
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentTriangleInf(start, end, triangle, pointIntersection1, pointIntersection2);
            if(flag == 2) return 2;
            else if(flag == 1) return 1;
            else return 0;
        }
        //2 point of triangle on infinity
        else if (triangle.p1().w() == 0 && triangle.p2().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentSegmentInf(start,end, triangle.p1(), triangle.p2(), pointIntersection1, pointIntersection2);
            if(flag == 2) return 2;
            else if(flag == 1) return 1;
            else return 0;
        } 
        else if (triangle.p1().w() == 0 && triangle.p3().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentSegmentInf(start,end, triangle.p1(), triangle.p3(), pointIntersection1, pointIntersection2);
            if(flag == 2) return 2;
            else if(flag == 1) return 1;
            else return 0;
        } 
        else if (triangle.p2().w() == 0 && triangle.p3().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentSegmentInf(start,end, triangle.p2(), triangle.p3(), pointIntersection1, pointIntersection2);
            if(flag == 2) return 2;
            else if(flag == 1) return 1;
            else return 0;
        }
        
        //1 point of triangle on infinity
        else if (triangle.p1().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentPointInf(start, end, triangle.p1());
            if(flag == 1)
            {
                pointIntersection1 = triangle.p1();
                return 1;
            }
            else return 0;
        } 
        else if (triangle.p2().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentPointInf(start, end, triangle.p2());
            if(flag == 1)
            {
                pointIntersection1 = triangle.p2();
                return 1;
            }
            else return 0;
        } 
        else if (triangle.p3().w() == 0) 
        {
            flag = ProjectiveConvexQuickHull::intersectSegmentPointInf(start, end, triangle.p3());
            if(flag == 1)
            {
                pointIntersection1 = triangle.p3();
                return 1;
            }
            else return 0;
        }
    }
}
int ProjectiveConvexQuickHull::isInsidePointTriangleInf(const ProjectiveCoord4d &pointCheck,const Triangle4dd &triangle4dd)
{
    Plane3d plane1 = Plane3d::FromPoints(Vector3dd::Zero(),triangle4dd.p1().xyz(), triangle4dd.p2().xyz());

    Vector3dd normal1 = triangle4dd.p1().xyz() ^ triangle4dd.p2().xyz();
    Vector3dd normal2 = triangle4dd.p3().xyz() ^ triangle4dd.p1().xyz();
    Vector3dd normal3 = triangle4dd.p2().xyz() ^ triangle4dd.p3().xyz();
    Vector3dd point = Vector3dd::Zero();
    double pointWeight1 = (normal1 & pointCheck.toVector()) - (normal1 & point);
    double pointWeight2 = (normal2 & pointCheck.toVector()) - (normal2 & point);
    double pointWeight3 = (normal3 & pointCheck.toVector()) - (normal3 & point);
    if (pointWeight1 > 0 ||pointWeight2 > 0 ||pointWeight3 > 0)
    {
        return false;
    }
    return true;
}
int ProjectiveConvexQuickHull::intersectSegmentTriangleInf(const ProjectiveCoord4d &start,const ProjectiveCoord4d &end, const Triangle4dd &triangle4dd, ProjectiveCoord4d &pointIntersection1,ProjectiveCoord4d &pointIntersection2 )
{
    ProjectiveCoord4d res1;
    ProjectiveCoord4d res2;
    
    vector<ProjectiveCoord4d> result;
    if(isInsidePointTriangleInf(start, triangle4dd)) result.push_back(start);
    if(isInsidePointTriangleInf(end, triangle4dd)) result.push_back(end);
    int flag = intersectSegmentSegmentInf(start, end, triangle4dd.p1(), triangle4dd.p2(), res1, res2);
    if(flag == 2)
    {
        pointIntersection1 = res1;
        pointIntersection2 = res2;
        return 2;
    }
    else if(flag == 1)
    {
        result.push_back(res1);
    }
    
    flag = intersectSegmentSegmentInf(start, end, triangle4dd.p2(), triangle4dd.p3(), res1, res2);
    if(flag == 2)
    {
        pointIntersection1 = res1;
        pointIntersection2 = res2;
        return 2;
    }
    else if(flag == 1)
    {
        result.push_back(res1);
    }
    
    flag = intersectSegmentSegmentInf(start, end, triangle4dd.p3(), triangle4dd.p1(), res1, res2);
    if(flag == 2)
    {
        pointIntersection1 = res1;
        pointIntersection2 = res2;
        return 2;
    }
    else if(flag == 1)
    {
        result.push_back(res1);
    }
    
    if(result.size() == 2)
    {
        pointIntersection1 = result[0];
        pointIntersection2 = result[1];
        return 2;
    }
    else if (result.size() == 1) 
    {
        pointIntersection1 = result[0];
        return 2;
    }
    else return 0;
}

int ProjectiveConvexQuickHull::intersectSegmentSegmentInf(const ProjectiveCoord4d &start1,const ProjectiveCoord4d &end1,const ProjectiveCoord4d &start2,const ProjectiveCoord4d &end2, ProjectiveCoord4d &pointIntersection1,ProjectiveCoord4d &pointIntersection2)
{

    Plane3d plane1 = Plane3d::FromPoints(Vector3dd::Zero(),start1.xyz(),end1.xyz());
    Plane3d plane2 = Plane3d::FromPoints(Vector3dd::Zero(),start2.xyz(),end2.xyz());
    if(plane1.normalised() == plane2.normalised()) 
    {
        vector<ProjectiveCoord4d> pointIntersections;
        if(intersectSegmentPointInf(start1, end1, start2)) pointIntersections.push_back(start2);
        if(intersectSegmentPointInf(start1, end1, end2)) pointIntersections.push_back(end2);
        if(intersectSegmentPointInf(start2, end2, start1)) pointIntersections.push_back(start1);
        if(intersectSegmentPointInf(start2, end2, end1)) pointIntersections.push_back(end1);
        if(pointIntersections.size() == 2)
        {
            pointIntersection1 = pointIntersections[0];
            pointIntersection2 = pointIntersections[1];
            return 2;
        }
        else if(pointIntersections.size() == 1)
        {
            pointIntersection1 = pointIntersections[0];
            return 1;
        }
        else return 0;
    }
    bool flag;
    ProjectiveCoord4d intersectRay = ProjectiveCoord4d(plane1.intersectWith(plane2,&flag).a, 0);
    if(intersectSegmentPointInf(start1, end1, intersectRay)==1 && intersectSegmentPointInf(start2, end2, intersectRay)==1)
    {
        pointIntersection1 = intersectRay;
        return 1;
    }
    return 0;
}
int ProjectiveConvexQuickHull::intersectSegmentPointInf(const ProjectiveCoord4d &start,const ProjectiveCoord4d &end, const ProjectiveCoord4d &point)
{
    if (start.xyz().angleTo(end.xyz()) == start.xyz().angleTo(point.xyz())+point.xyz().angleTo(end.xyz())) return 1;
    else return 0;
}


ConvexPolyhedronP::ConvexPolyhedronP(){}

ConvexPolyhedronP::ConvexPolyhedronP(const vector<ProjectiveCoord4d> &newVertices)
{
    this->vertices = newVertices;
    ProjectiveConvexQuickHull::HullFaces hullFaces = ProjectiveConvexQuickHull::quickHull(vertices);
    for(const ProjectiveConvexQuickHull::HullFace &newFace : hullFaces)
    {
        this->faces.push_back(newFace.plane);
    }
}

ProjectiveConvexQuickHull::HullFaces ConvexPolyhedronP::intersectWith(const ConvexPolyhedronP &poly1, const ConvexPolyhedronP &poly2)
{
    return ProjectiveConvexQuickHull::intersect(ProjectiveConvexQuickHull::quickHull(poly1.vertices), ProjectiveConvexQuickHull::quickHull(poly2.vertices));
}

void ProjectiveConvexQuickHull::HullFaces::addToMesh(Mesh3D &mesh)
{
    for (const ProjectiveConvexQuickHull::HullFace &f : *this )
    {
        Triangle4dd tri4dd = f.plane;
        mesh.addTriangle(Triangle3dd(tri4dd.p1().toVector(), tri4dd.p2().toVector(),tri4dd.p3().toVector()));
    }
}
void ConvexPolyhedronP::addToMesh(Mesh3D &mesh)
{
    for(const Triangle4dd f : this->faces)
    {
        mesh.addTriangle(Triangle3dd(f.p1().toVector(), f.p2().toVector(),f.p3().toVector()));
    }
}
} // namespace corecvs
