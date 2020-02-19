#ifndef MESHCLICKER_H
#define MESHCLICKER_H

#include "geometry/mesh/mesh3d.h"

namespace corecvs {

/* Please check raytraceRenderer for some related code */

/*class ClickIntersection {

    double
};*/


class MeshClicker
{
public:

    Mesh3D *targetMesh = NULL;
    MeshClicker();

    double vertexTolerance = 4;
    double   edgeTolerance = 4;


    bool vertexClicked( double &t, int vertexNum);
    bool edgeClicked  ( double &t, int edgeNum);
    bool faceClicked  ( double &t, int vertexNum);


};


} // namespace corecvs

#endif // MESHCLICKER_H
