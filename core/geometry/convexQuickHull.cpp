#include "convexQuickHull.h"
#include "core/geometry/mesh/mesh3d.h"

using namespace std;

namespace corecvs {

bool operator ==(const Triangle3dd &a, const Triangle3dd &b) {
    vector<Vector3dd> plane1 = { b.p1(), b.p2(), b.p3() };
    auto findX = find(plane1.begin(), plane1.end(), a.p1());
    auto findY = find(plane1.begin(), plane1.end(), a.p2());
    auto findZ = find(plane1.begin(), plane1.end(), a.p3());
    return (findX != plane1.end()) && (findY != plane1.end()) && (findZ != plane1.end());
}
bool operator !=(const Triangle3dd &a, const Triangle3dd &b) {
    return !(a == b);
}

bool operator ==(const ConvexQuickHull::HullFace &a, const ConvexQuickHull::HullFace &b)
{
    return a.plane == b.plane;
}

double tripleProd(const Vector3dd &v1, const Vector3dd &v2, const Vector3dd &v3) {
    return  v1 & (v2 ^ v3);
}

double pointLineDist(const Vector3dd &lineP1, const Vector3dd &lineP2, const Vector3dd &point) {
    Vector3dd lineVect = lineP2 - lineP1;
    return (lineVect ^ (point - lineP1)).l2Metric() / lineVect.l2Metric();
}

double pointPlaneDist(const Vector3dd &planeP1, const Vector3dd &planeP2, const Vector3dd &planeP3, const Vector3dd &point) {
    Vector3dd baseV1 = planeP2 - planeP1;
    Vector3dd baseV2 = planeP3 - planeP1;
    return tripleProd(baseV1, baseV2, point - planeP1) / ((baseV1 ^ baseV2).l2Metric());
}

ConvexQuickHull::Vertices createSimplex(const ConvexQuickHull::Vertices& listVertices) {
    Vector3dd first = listVertices.front();
    Vector3dd EP[6] = {first, first, first, first, first, first};

    for (auto vertex : listVertices) {
        if (vertex.x() <= EP[0][0]) EP[0] = vertex;
        if (vertex.x() >= EP[1][0]) EP[1] = vertex;
        if (vertex.y() <= EP[2][1]) EP[2] = vertex;
        if (vertex.y() >= EP[3][1]) EP[3] = vertex;
        if (vertex.z() <= EP[4][2]) EP[4] = vertex;
        if (vertex.z() >= EP[5][2]) EP[5] = vertex;
    }

    double maxDist = 0;
    Vector3dd triangleP1 = EP[0], triangleP2 = EP[0], triangleP3 = EP[0];
    for (auto point1 : EP)
        for (auto point2 : EP) {
            double dist = (point2 - point1).l2Metric();
            if (dist > maxDist) {
                maxDist = dist;
                triangleP1 = point1;
                triangleP2 = point2;
            }
        }

    maxDist = 0;
    for (auto point : EP) {
        double dist = pointLineDist(triangleP1, triangleP2, point);
        if (dist > maxDist) {
            maxDist = dist;
            triangleP3 = point;
        }
    }

    maxDist = 0;
    Vector3dd apex = EP[0];
    for (auto point : listVertices) {
        double dist = abs(pointPlaneDist(triangleP1, triangleP2, triangleP3, point));
        if (dist > maxDist) {
            maxDist = dist;
            apex = point;
        }
    }
    ConvexQuickHull::Vertices Res;
    if (pointPlaneDist(triangleP1, triangleP2, triangleP3, apex) > 0)
         Res = { triangleP1, triangleP3, triangleP2, apex };
    else Res = { triangleP1, triangleP2, triangleP3, apex };
    return Res;
}




ConvexQuickHull::HullFaces ConvexQuickHull::quickHull(const ConvexQuickHull::Vertices &listVertices, double epsilon)
{
    queue<HullFace> queue;

    if (listVertices.size() < 4)
    {
        return ConvexQuickHull::HullFaces();
    }

    Vertices simplex = createSimplex(listVertices);
    Vertices uniqueSimplex;

    for (Vector3dd& elem : simplex) {
        if (find(uniqueSimplex.begin(), uniqueSimplex.end(), elem) == uniqueSimplex.end())
        {
            uniqueSimplex.push_back(elem);
        }
    }

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

    addPointsToFaces(faces.data(), (unsigned long)faces.size(), listVertices, epsilon);
    for (auto face : faces) {
        if (!face.points.empty()) queue.push(face);
    }

    while (!queue.empty()) {
        HullFace face = queue.front();
        queue.pop();

        if (!face.points.empty()) {
            double maxDist = -1;
            Vector3dd furthest = {0, 0, 0};
            for (auto point : face.points) {
                double dist = pointFaceDist(face.plane, point);
                if (dist > maxDist) {
                    maxDist = dist;
                    furthest = point;
                }
            }

            Vertices listUnclaimedVertices;
            HullFaces newFaces;

            for (HullFace &curFace : faces)
            {
                if (faceIsVisible(furthest, curFace, epsilon))
                {
                    listUnclaimedVertices.insert(listUnclaimedVertices.end(), curFace.points.begin(), curFace.points.end());

                    HullFaces tmpFaces;
                    tmpFaces.reserve(3);
                    tmpFaces.emplace_back(curFace.plane.p3(), furthest, curFace.plane.p2());
                    tmpFaces.emplace_back(curFace.plane.p1(), curFace.plane.p2(), furthest);
                    tmpFaces.emplace_back(curFace.plane.p1(), furthest, curFace.plane.p3());


                    for (HullFace &tmpFace : tmpFaces) {
                        auto alreadyExists = find(newFaces.begin(), newFaces.end(), tmpFace);
                        if (alreadyExists != newFaces.end()) {
                            newFaces.erase(alreadyExists);
                        } else {
                            newFaces.push_back(tmpFace);
                        }
                    }                   
                    curFace.toDelete = true;
                }
            }

            faces.erase(remove_if(
                            faces.begin(),
                            faces.end(),
                            [](HullFace &face) {return face.toDelete;}
                    ), faces.end());

            addPointsToFaces(newFaces.data(), (unsigned long)newFaces.size(), listUnclaimedVertices, epsilon);
            for (auto &newFace : newFaces) {
                faces.push_back(newFace);
                queue.push(newFace);
            }
        }
    }
    return faces;
}

/**
 * Slow, but obvious method. Method with duals so far is not implemented
 **/
ConvexQuickHull::HullFaces ConvexQuickHull::intersect(const ConvexQuickHull::HullFaces &v1, const ConvexQuickHull::HullFaces &v2)
{
    vector<Vector3dd> intersection;

    vector<Vector3dd> p1 = v1.getPoints();
    vector<Vector3dd> p2 = v2.getPoints();

    for (Vector3dd &v : p1)
    {
        if (v2.isInside(v)) intersection.push_back(v);
    }

    for (Vector3dd &v : p2)
    {
        if (v1.isInside(v)) intersection.push_back(v);
    }

    /* Oh well. This would take a lot of time */

    for (const ConvexQuickHull::HullFace &f : v1)
    {
        const Triangle3dd &triang = f.plane;
        Ray3d r[3] = {triang.r1(), triang.r2(), triang.r3()};
        for (uint i = 0; i < CORE_COUNT_OF(r); i++)
        {
            double t1, t2;
            bool flag = r[i].clip<ConvexQuickHull::HullFaces>(v2, t1, t2);
            if (!flag)
                continue;
            if (t1 >=0 && t1 <= 1.0) intersection.push_back(r[i].getPoint(t1));
            if (t2 >=0 && t2 <= 1.0) intersection.push_back(r[i].getPoint(t2));
        }
    }

    for (const ConvexQuickHull::HullFace &f : v2)
    {
        const Triangle3dd &triang = f.plane;
        Ray3d r[3] = {triang.r1(), triang.r2(), triang.r3()};
        for (uint i = 0; i < CORE_COUNT_OF(r); i++)
        {
            double t1, t2;
            bool flag = r[i].clip<ConvexQuickHull::HullFaces>(v1, t1, t2);
            if (!flag)
                continue;
            if (t1 >=0 && t1 <= 1.0) intersection.push_back(r[i].getPoint(t1));
            if (t2 >=0 && t2 <= 1.0) intersection.push_back(r[i].getPoint(t2));
        }
    }

    //SYNC_PRINT(("ConvexQuickHull::intersect. Base points %d \n", intersection.size()));

    return ConvexQuickHull::quickHull(intersection);
}

double ConvexQuickHull::pointFaceDist(const Triangle3dd &face, const Vector3dd &point) {
    return pointPlaneDist(face.p1(), face.p2(), face.p3(), point);
}

bool ConvexQuickHull::faceIsVisible(const Vector3dd &eyePoint, const ConvexQuickHull::HullFace &face, double eps) {
    //return pointFaceDist(face.plane, eyePoint) > eps;
    return ((eyePoint - face.plane.p1()) & face.normal) > eps;
}

void ConvexQuickHull::addPointsToFaces(HullFace *faces, unsigned long faces_count, const Vertices &listVertices, double eps)
{
    //SYNC_PRINT(("ConvexQuickHull::addPointsToFaces() : %d %d\n", faces_count, listVertices.size()));
    for (const Vector3dd &vertex : listVertices)
    {
        for (unsigned long i = 0; i < faces_count; i++) {
            HullFace &face = faces[i];
            if (/*(vertex != face.plane.p1()) &&
                (vertex != face.plane.p2()) &&
                (vertex != face.plane.p3()) &&*/
                 faceIsVisible(vertex, face, eps))
            {
                faces[i].points.push_back(vertex);
                break;
            }
        }
    }
}

ConvexQuickHull::HullFaces::HullFaces(Mesh3D &mesh)
{
    reserve(mesh.faces.size());
    for (size_t i = 0; i < mesh.faces.size(); i++ ) {
        Triangle3dd t = mesh.getFaceAsTrinagle(i);
        push_back(HullFace(t));
    }
}

void ConvexQuickHull::HullFaces::addToMesh(Mesh3D &mesh, bool drawNormals)
{
    for (const ConvexQuickHull::HullFace &f : *this )
    {
        mesh.addTriangle(f.plane);
    }

    if (drawNormals) {
        for (const ConvexQuickHull::HullFace &f : *this )
        {
            Vector3dd center = f.plane.getCenter();
            mesh.addLine(center, center + f.normal);
        }
    }
}

vector<Vector3dd> ConvexQuickHull::HullFaces::getPoints(bool dedup) const
{
    vector<Vector3dd> toReturn;
    toReturn.reserve(size() * 3);
    for (const ConvexQuickHull::HullFace &f : *this )
    {
        toReturn.push_back(f.plane.p1());
        toReturn.push_back(f.plane.p2());
        toReturn.push_back(f.plane.p3());
    }
    if (dedup) {
        std::sort(toReturn.begin(), toReturn.end(), [](Vector3dd &a, Vector3dd &b)
            {
                if (a.x() < b.x()) return true;
                if (a.x() > b.x()) return false;

                if (a.y() < b.y()) return true;
                if (a.y() > b.y()) return false;

                if (a.z() < b.z()) return true;
                return false;
            }
        );
        toReturn.erase(std::unique(toReturn.begin(), toReturn.end()), toReturn.end());
    }

    return toReturn;
}

void ConvexQuickHull::HullFaces::reorientNormals(int targetSign)
{
    vector<Vector3dd> points = getPoints(true);

    int sign = targetSign;

    int flips = 0;

    for (ConvexQuickHull::HullFace &f : *this )
    {
        Plane3d plane = f.getPlane();

        for (Vector3dd &p : points)
        {
            double newSign = plane.pointWeight(p);


            if (newSign >  1e-7 ) {
                if (sign  > 0) { break; }
                if (sign == 0) { sign = 1; break; }

                f.normal = -f.normal;
                flips++;
                break;
            }

            if (newSign < -1e-7 ) {
                if (sign  < 0) {break; }
                if (sign == 0) {sign = -1; break; }

                f.normal = -f.normal;
                flips++;
                break;
            }
        }
    }
    cout << "sign :" << sign  << endl;
    cout << "filps:" << flips << endl;
}

} // namespace corecvs
