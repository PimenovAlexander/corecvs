#ifndef MESHFILTER_H
#define MESHFILTER_H

#include "core/geometry/mesh/mesh3DDecorated.h"

namespace corecvs {

class MeshFilter
{
public:
    static void removeDuplicatedFaces(Mesh3DDecorated &mesh);

    static void removeUnreferencedVertices(Mesh3DDecorated &mesh);

    static void removeIsolatedPieces(Mesh3DDecorated &mesh,  unsigned minCountOfFaces);

    static void removeZeroAreaFaces(Mesh3DDecorated &mesh);

    static void removeDuplicatedVertices(Mesh3DDecorated &mesh);
};

};

#endif // MESHFILTER_H
