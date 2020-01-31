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



    friend std::ostream& operator << (std::ostream &out, SceneShadedFaceCache &toPrint)
    {
        out << "  Sizes:" << endl;

        out << "    facePositions   :" << toPrint.facePositions.size()    << endl;
        out << "    faceVertexColors:" << toPrint.faceVertexColors.size() << endl;
        out << "    faceColors      :" << toPrint.faceColors.size()       << endl;
        out << "    faceNormals     :" << toPrint.faceNormals.size()      << endl;
        out << "    faceTexCoords   :" << toPrint.faceTexCoords.size()    << endl;
        out << "    faceTexNums     :" << toPrint.faceTexNums.size()      << endl;
        out << "    faceIds         :" << toPrint.faceIds.size()          << endl;

        return out;
    }


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
    void cache     (Mesh3DDecorated *mesh, const Draw3dParameters &mParameters);
    void clearCache(void);

    friend std::ostream& operator << (std::ostream &out, SceneShadedOpenGLCache &toPrint)
    {
        out << "Scene cache:" << endl;

        out << "  positions       :" << toPrint.positions.size()        << endl;
        out << "  edgePositions   :" << toPrint.edgePositions.size()    << endl;
        out << "  edgeVertexColors:" << toPrint.edgeVertexColors.size() << endl;
        out << "  edgeColors      :" << toPrint.edgeColors.size()       << endl;
        out << "  edgeIds         :" << toPrint.edgeIds.size()          << endl;

        out << "  faces by Material:" << endl;

        for (size_t id = 0; id < toPrint.faceCache.size(); id++)
        {
            out << "    ID:" << id << endl;
            out << toPrint.faceCache[id] << endl;
        }

        return out;

    }

    static size_t getMaterialID(Mesh3DDecorated *mesh, size_t &mFace);
};

} // namespace corecvs;

#endif // MESHCACHE_H
