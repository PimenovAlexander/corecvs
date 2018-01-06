#ifndef CONVEXQUICKHULL_H
#define CONVEXQUICKHULL_H


#include <algorithm>
#include <array>
#include "queue"
#include "../corecvs/core/math/vector/vector3d.h"
#include "../corecvs/core/geometry/polygons.h"

using namespace corecvs;
using namespace std;

typedef vector<Vector3dd> vertices;

typedef struct {
    Triangle3dd plane;
    vertices points;
} tFace;


typedef vector<tFace> tFaces;

class ConvexQuickHull
{
public:

    static tFaces quickHull(const vertices& listVertices, double epsilon);

protected:

    static double pointFaceDist(const Triangle3dd &face, const Vector3dd &point);

    static bool faceIsVisible(const Vector3dd &eyePoint, const tFace &face, double eps);

    static void addPointsToFaces(tFace* faces, unsigned long faces_count, const vertices &listVertices, double eps);

};


#endif // CONVEXQUICKHULL_H
