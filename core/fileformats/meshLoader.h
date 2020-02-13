#ifndef MESH_LOADER_H
#define MESH_LOADER_H
/**
 * \file meshLoader.h
 * \brief This is a header for Mesh file format reader
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */
#include <string>

#include "core/utils/global.h"
#include "core/geometry/mesh/mesh3d.h"

namespace corecvs {

class MeshLoader
{
public:
    enum DumpFormat {
        FORMAT_PLY,
        FORMAT_STL,
        FORMAT_OBJ,
        FORMAT_XYZ
    };

    static inline const char *getName(const DumpFormat &value)
    {
        switch (value)
        {
            case FORMAT_PLY : return "FORMAT_PLY"; break;
            case FORMAT_STL : return "FORMAT_STL"; break;
            case FORMAT_OBJ : return "FORMAT_OBJ"; break;
            case FORMAT_XYZ : return "FORMAT_XYZ"; break;
        }
        return "MeshLoader::getName(): Not in range";
    }

    bool trace;
    bool binary = false;

    MeshLoader();

    bool load(Mesh3D *mesh, const std::string &fileName);

    bool save(Mesh3D *mesh, const std::string &fileName);

    //bool save(Mesh3D *mesh, const DumpFormat &format);

    static std::string extentionList();
};

} // namespace corecvs

#endif // MESH_LOADER_H
