#ifndef MESHCACHE_H
#define MESHCACHE_H

#include "core/buffers/rgb24/rgbColor.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/xml/generated/draw3dParameters.h"

namespace corecvs {


class SceneShadedFaceCache {
public:
    std::vector<Vector3df> facePositions;
    std::vector<RGBColor>  faceVertexColors;
    std::vector<RGBColor>  faceColors;
    std::vector<Vector3df> faceNormals;
    std::vector<Vector2df> faceTexCoords;
    std::vector<uint32_t>  faceTexNums;
    std::vector<uint32_t>  faceIds;


};

class SceneShadedOpenGLCache {
public:
    /* Caches in OpenGL format*/
    /* For vertex draw */
    vector<Vector3df> positions;

    /* For edges draw */
    vector<Vector3df> edgePositions;
    vector<RGBColor>  edgeVertexColors;
    vector<RGBColor>  edgeColors;
    vector<uint32_t>  edgeIds;

    /* For face draw */
    vector<SceneShadedFaceCache> faceCache;

    /**/
    void cache(Mesh3DDecorated *mesh, const Draw3dParameters &mParameters);
    void clear(void);

    /*Parameters. Rework this*/

};

} // namespace corecvs;

#endif // MESHCACHE_H
