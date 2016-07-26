#include "polylinemesh.h"

namespace corecvs {

/*
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
                if (p_line.colors.size() >= b2 + t)
                {
                    mesh.setColor(p_line.colors[b2 + t]);
                }
                mesh.addTriangle(p_line.points[b2 + t], polyline.points[b1 + t], polyline.points[b1 + t + 1]);
                mesh.addTriangle(p_line.points[b2 + t], polyline.points[b1 + t + 1], p_line.points[b2 + t + 1]);

            }
        }
    }

    polyline = p_line;
}
*/

void PolylineMesh::addPolyline(PolyLine p_line)
{
    if (polyline.points.size() != 0)
    for(int i = 0; i < p_line.points.size() - 1; i++)
    {
        Vector3dd v1(p_line.points[i]), v1n(p_line.points[i + 1]);
        Vector3dd dv = v1 - v1n;

        bool l1(true), l2(true);

        if (dv.l2Metric() > MAXD)
            l1 = false;

        Vector3dd v0, v0n;

        v0 = polyline.points[0];
        int n = 0;
        for(int k = 1; k < polyline.points.size() - 1; k++)
        {
            Vector3dd v = polyline.points[k];
            Vector3dd dvv1 = v - v1, dv1v0 = v0 - v1;

            if (dvv1[0] * dvv1[0] < dv1v0[0] * dv1v0[0])
            {
                v0 = v;
                n = k;
            }
        }

        Vector3dd dv1v0 = v1 - v0;
        if (dv1v0.l2Metric() > MAXD)
            continue;

        v0n = polyline.points[(n + 1 < polyline.points.size()) ? n + 1 : n];

        Vector3dd dv2 = v0n - v0;
        if (dv2.l2Metric() > MAXD)
            l2 = false;

        if (l1 && l2)
        {
            if (p_line.colors.size() > i - 1)
                mesh.setColor(p_line.colors[i]);

            mesh.addTriangle(v1,v1n,v0);
            mesh.addTriangle(v0,v0n,v1n);
        }
        else
            if (l1)
            {
                if (p_line.colors.size() > i - 1)
                    mesh.setColor(p_line.colors[i]);

                mesh.addTriangle(v1,v0,v1n);
            }
            else
                if (l2)
                {
                    if (p_line.colors.size() > i - 1)
                        mesh.setColor(p_line.colors[i]);

                    mesh.addTriangle(v1,v0,v0n);
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
