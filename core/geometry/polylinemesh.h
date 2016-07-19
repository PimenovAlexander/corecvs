#ifndef POLYLINEMESH_H
#define POLYLINEMESH_H

#include "polyLine.h"
#include "mesh3d.h"

namespace corecvs {
    class PolylineMesh{
    public:
        Mesh3D mesh;
        PolyLine polyline;
        bool exist;

        PolylineMesh():
            exist(false)
        {
            //mesh.hasTexCoords = true;
        }

        void addPolyline(PolyLine p_line);

        void clear();
    };
}
#endif // POLYLINEMESH_H
