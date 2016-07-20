#include "polylinemesh.h"

namespace corecvs {

void PolylineMesh::addPolyline(PolyLine p_line)
{
    if (p_line.points.size() == 0)
    {
        polyline = p_line;
        exist = false;
        return;
    }

    if(!exist){
        polyline = p_line;
        exist = true;
        return;
    }

    for(uint i = 0; i < polyline.begins.size();i++)
    {
        for(uint k = 0; k < p_line.breaks.size(); k++)
        {
            Vector3dd vb1 = polyline.points[polyline.begins[i]];
            Vector3dd ve1 = polyline.points[polyline.breaks[i]];
            Vector3dd vb2 = p_line.points[p_line.begins[k]];
            Vector3dd ve2 = p_line.points[p_line.breaks[k]];

            if ((vb1[0] > ve2[0]) || (ve1[0] < vb2[0]))
                continue;

            Vector2d32 v1 = polyline.closestPointsGenerate(p_line, i, k);
            Vector2d32 v2 = p_line.closestPointsGenerate(polyline, k, i);

            bool v_b1 = (vb1[0] > vb2[0]);
            bool v_b2 = (ve1[0] < ve2[0]);

            uint32_t b1, e1, b2, e2;

            if (v_b1)
            {
                b1 = polyline.begins[i];
                b2 = v1[0];
            }else{
                b1 = v2[0];
                b2 = p_line.begins[k];
            }

            if (v_b2)
            {
                e1 = polyline.breaks[i];
                e2 = v1[1];
            }else{
                e1 = v2[1];
                e2 = p_line.breaks[k];
            }

            int num1 = e1 - b1;
            int num2 = e2 - b2;

            if ((num1 < 0) || (num2 < 0))
                continue;

            if ((b1 + num1 >= polyline.points.size()) || (b2 + num2 >= p_line.points.size()))
                continue;

            if (num1 > num2)
            {
                for (int t = 0; t < num1 - num2; t++)
                    mesh.addTriangle(p_line.points[b2], polyline.points[b1 + t], polyline.points[b1 + t + 1]);
                num1 = num2;
                b1 += num1 - num2;
            }else
                if (num2 > num1)
                {
                    for (int t = 0; t < num2 - num1; t++)
                        mesh.addTriangle(polyline.points[b1], p_line.points[b2 + t], p_line.points[b2 + t + 1]);
                    num2 = num1;
                    b2 += num2 - num1;
                }

            for (int t = 0; t < num2; t++)
            {
                mesh.addTriangle(p_line.points[b2 + t], polyline.points[b1 + t], polyline.points[b1 + t + 1]);
                mesh.addTriangle(p_line.points[b2 + t], polyline.points[b1 + t + 1], p_line.points[b2 + t + 1]);
            }
        }
    }

    polyline = p_line;
}

void PolylineMesh::clear()
{
    mesh.clear();
    polyline.clear();
}

} // namespace corecvs
