#pragma once
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "utils/global.h"

#include "geometry/mesh/mesh3d.h"

namespace corecvs {

using std::vector;

class PLYLoader {
public:
    enum PlyFormat {
        ASCII,
        BINARY_LITTLE_ENDIAN,
        OTHER
    };

    enum PropType  {
        PROP_TYPE_FLOAT,
        PROP_TYPE_UCHAR,
        PROP_TYPE_INT,
        PROP_TYPE_LIST,
        PROP_TYPE_CORRUPT,
    };

    enum PropName  {
        PROP_NAME_X,
        PROP_NAME_Y,
        PROP_NAME_Z,

        PROP_NAME_RED,
        PROP_NAME_GREEN,
        PROP_NAME_BLUE,

        PROP_NAME_CLUSTER,

        PROP_NAME_VERTEX1,
        PROP_NAME_VERTEX2,        

        PROP_NAME_VERTEX_INDEX,

        PROP_NAME_CORRUPT
    };

    struct Prop {
        PropType type;
        PropName name;

        Prop() :
            type(PROP_TYPE_CORRUPT),
            name(PROP_NAME_CORRUPT)
        {}

        static const char *typeToStr(PropType type);
        static const char *nameToStr(PropName name);

    };

    enum ObjType {
        OBJ_VERTEX,
        OBJ_FACE,
        OBJ_EDGE,
        OBJ_LAST
    };

    bool trace;

    friend std::istream &operator >> (std::istream &in, Prop &toLoad);

    PLYLoader() :
        trace (true)
    {}

    int loadPLY(std::istream &input, Mesh3D &mesh);
    int savePLY(std::ostream &out, Mesh3D &mesh, PlyFormat format = PlyFormat::ASCII);

    virtual ~PLYLoader();
};

} // namespace corecvs
/* EOF */
