#ifndef POLYLINE_H
#define POLYLINE_H
#include "vector3d.h"

namespace corecvs {

class PolyLine{
public:
    vector<Vector3dd> points;
    vector<int32_t> breaks;
    vector<int32_t> begins;

    PolyLine(vector<Vector3dd> p);
    PolyLine(){}

    Vector2d32 closestPointsGenerate(PolyLine p_line, int n1, int n2);
};

}
#endif // POLYLINE_H
