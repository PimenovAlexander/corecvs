#ifndef POLYLINEMESH_H
#define POLYLINEMESH_H

#include "polyLine.h"
#include "mesh3d.h"

namespace corecvs {
    class polylineMesh{
    public:
        Mesh3D mesh;
        polyLine polyline;
        bool exist;

        polylineMesh(): exist(false){
            mesh.hasTexCoords = true;
        }

        void addPolyline(polyLine p_line);
    };
}
#endif // POLYLINEMESH_H
