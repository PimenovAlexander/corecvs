#include "polyLine.h"
#include "vector3d.h"

#define MAXD 1.5

namespace corecvs {
    polyLine::polyLine(vector<Vector3dd> p)
    {
        points = p;
        begins.push_back(0);

        for (int i = 0; i < points.size(); i++)
        {
            if (i > 0)
            {
                Vector3dd dv = p[i] - p[i -1];
                double d = dv[0] * dv[0] + dv[1] * dv[1];

                if (d > MAXD)
                {
                    breaks.push_back(i - 1);
                    begins.push_back(i);
                }
            }
        }

        breaks.push_back(points.size() - 1);
    }

    Vector2d32 polyLine::cl_pts_gen(polyLine p_line, int n1, int n2)
    {
        Vector3dd v1 = points[begins[n1]];
        Vector3dd v2 = points[breaks[n1]];

        int closeset1, closeset2;
        double best_diff1, best_diff2;
        bool v1_f, v2_f;
        v1_f = v2_f = true;

        for(int i = p_line.begins[n2]; (i <= p_line.breaks[n2]) && (v1_f || v2_f); i++)
        {
            if(v1_f)
            {
                Vector3dd dv = v1 - p_line.points[i];
                double d = dv[0] * dv[0] + dv[1] * dv[1];

                if ((i == p_line.begins[n2]) || (d < best_diff1))
                {
                    closeset1 = i;
                    best_diff1 = d;
                } else
                    if ((i != p_line.begins[n2]) && (d > best_diff1))
                        v1_f = false;
            }

            if(v2_f)
            {
                Vector3dd dv = v2 - p_line.points[i];
                double d = dv[0] * dv[0] + dv[1] * dv[1];

                if ((i == p_line.begins[n2]) || (d < best_diff2))
                {
                    closeset2 = i;
                    best_diff2 = d;
                } else
                    if ((i != p_line.begins[n2]) && (d > best_diff2))
                        v2_f = false;
            }
        }

        return Vector2d32(closeset1, closeset2);
    }
}
