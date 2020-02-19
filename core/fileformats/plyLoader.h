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
        PROP_TYPE_DOUBLE,
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
        PROP_NAME_ALPHA,

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


        double getDouble(std::istream &input) {
            double toReturn = 0;
            switch (type) {
                case PROP_TYPE_FLOAT: {
                    float f = 0.0f;
                    input.read((char *)&f, sizeof(f)); toReturn = f;
                    break;
                };
                case PROP_TYPE_DOUBLE: {
                    double d = 0.0;
                    input.read((char *)&d, sizeof(d)); toReturn = d;
                    break;
                }
                case PROP_TYPE_UCHAR: {
                    unsigned char c = 0;
                    input.read((char *)&c, sizeof(c)); toReturn = c;
                    break;
                }
                case PROP_TYPE_INT: {
                    int32_t i = 0;
                    input.read((char *)&i, sizeof(i)); toReturn = i;
                    break;
                }
                default:
                    break;
            }
            return toReturn;
        }
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
    int savePLY(std::ostream &out, Mesh3D &mesh, PlyFormat format = PlyFormat::ASCII, bool useDouble = false, bool forceNoAlpha = false);

    virtual ~PLYLoader();
};

} // namespace corecvs
/* EOF */
