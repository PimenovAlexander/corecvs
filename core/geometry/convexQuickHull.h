#ifndef CONVEXQUICKHULL_H
#define CONVEXQUICKHULL_H


#include <algorithm>
#include <array>
#include "queue"
#include "core/math/vector/vector3d.h"
#include "core/geometry/polygons.h"

using namespace corecvs;
using namespace std;

class ConvexQuickHull
{
public:
    typedef vector<Vector3dd> vertices;

    struct HullFace {
        Triangle3dd plane;
        vertices points;

        HullFace(Triangle3dd plane) : plane(plane)
        {}
    };

    typedef vector<HullFace> HullFaces;

    static HullFaces quickHull(const vertices& listVertices, double epsilon);

protected:

    static double pointFaceDist(const Triangle3dd &face, const Vector3dd &point);

    static bool faceIsVisible(const Vector3dd &eyePoint, const HullFace &face, double eps);

    static void addPointsToFaces(HullFace* faces, size_t facesCount, const vertices &listVertices, double eps);

};


#endif // CONVEXQUICKHULL_H
