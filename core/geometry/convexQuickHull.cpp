#include "convexQuickHull.h"

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

Vector3dd createVect(const Vector3dd &p1, const Vector3dd &p2) {
    return { p2.x() - p1.x(), p2.y() - p1.y(), p2.z() - p1.z() };
}


double tripleProd(const Vector3dd &v1, const Vector3dd &v2, const Vector3dd &v3) {
    return  v1 & (v2 ^ v3);
}

double pointLineDist(const Vector3dd &lineP1, const Vector3dd &lineP2, const Vector3dd &point) {
    Vector3dd lineVect = lineP2 - lineP1;
    return (lineVect ^ (point - lineP1)).l2Metric() / lineVect.l2Metric();
}

double pointPlaneDist(const Vector3dd &planeP1, const Vector3dd &planeP2, const Vector3dd &planeP3, const Vector3dd &point) {
    Vector3dd baseV1 = createVect(planeP1, planeP2);
    Vector3dd baseV2 = createVect(planeP1, planeP3);
    return tripleProd(baseV1, baseV2, point - planeP1) / ((baseV1 ^ baseV2).l2Metric());
}

ConvexQuickHull::vertices createSimplex(const ConvexQuickHull::vertices& listVertices) {
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
    ConvexQuickHull::vertices Res;
    if (pointPlaneDist(triangleP1, triangleP2, triangleP3, apex) > 0)
         Res = { triangleP1, triangleP3, triangleP2, apex };
    else Res = { triangleP1, triangleP2, triangleP3, apex };
    return Res;
}




ConvexQuickHull::HullFaces ConvexQuickHull::quickHull(const ConvexQuickHull::vertices &listVertices, double epsilon) {
    queue<HullFace> Queue;
    vertices simplex = createSimplex(listVertices);
    vertices uniqueSimplex;
    for (auto elem : simplex)
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

    HullFaces faces = {{{simplex[0], simplex[1], simplex[2]}},
                    {{simplex[0], simplex[2], simplex[3]}},
                    {{simplex[1], simplex[3], simplex[2]}},
                    {{simplex[0], simplex[3], simplex[1]}}};
    addPointsToFaces(faces.data(), faces.size(), listVertices, epsilon);
    for (auto face : faces)
        if (!face.points.empty()) Queue.push(face);

    while (!Queue.empty()) {
        HullFace face = Queue.front();
        Queue.pop();
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

            vertices listUnclaimedVertices;
            HullFaces newFaces;
            HullFaces needToDelete;
            for (auto curFace : faces)
                if (faceIsVisible(furthest, curFace, epsilon)) {
                    for (auto point : curFace.points)
                        listUnclaimedVertices.push_back(point);
                    HullFaces tmpFaces = {{{curFace.plane.p3(), furthest, curFace.plane.p2()}},
                                       {{curFace.plane.p1(), curFace.plane.p2(), furthest}},
                                       {{curFace.plane.p1(), furthest, curFace.plane.p3()}}};
                    for (auto &tmpFace : tmpFaces) {
                        auto alreadyExists = find(newFaces.begin(), newFaces.end(), tmpFace);
                        if (alreadyExists != newFaces.end())
                            newFaces.erase(alreadyExists);
                        else
                            newFaces.push_back(tmpFace);
                    }
                    needToDelete.push_back(curFace);
                }

            for (auto &iter : needToDelete)
                faces.erase(remove(faces.begin(), faces.end(), iter), faces.end());

            addPointsToFaces(newFaces.data(), newFaces.size(), listUnclaimedVertices, epsilon);
            for (auto &newFace : newFaces) {
                faces.push_back(newFace);
                Queue.push(newFace);
            }
        }
    }
    return faces;
}

double ConvexQuickHull::pointFaceDist(const Triangle3dd &face, const Vector3dd &point) {
    return pointPlaneDist(face.p1(), face.p2(), face.p3(), point);
}

bool ConvexQuickHull::faceIsVisible(const Vector3dd &eyePoint, const ConvexQuickHull::HullFace &face, double eps) {
    return pointFaceDist(face.plane, eyePoint) > eps;
}

void ConvexQuickHull::addPointsToFaces(HullFace *faces, size_t facesCount, const vertices &listVertices, double eps) {
    for (auto vertex : listVertices)
        for (size_t i = 0; i < facesCount; i++) {
            HullFace face = faces[i];
            if ((vertex != face.plane.p1()) && (vertex != face.plane.p2()) && (vertex != face.plane.p3()) && faceIsVisible(vertex, face, eps)) {
                faces[i].points.push_back(vertex);
                break;
            }
        }
}
