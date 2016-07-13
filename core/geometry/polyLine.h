#ifndef POLYLINE_H
#define POLYLINE_H
#include "vector3d.h"

namespace corecvs {
    class polyLine{
    public:
        vector<Vector3dd> points;
        vector<int32_t> breaks;
        vector<int32_t> begins;

        polyLine(vector<Vector3dd> p);
        polyLine(){}

        Vector2d32 cl_pts_gen(polyLine p_line, int n1, int n2);
    };
}
#endif // POLYLINE_H
