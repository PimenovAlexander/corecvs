#ifndef MESH_LOADER_H
#define MESH_LOADER_H
/**
 * \file ppmLoader.h
 * \brief This is a header for PPM file format reader
 *
 * \ingroup cppcorefiles
 * \date Jun 22, 2010
 * \author alexander
 */

#include <string>


#include "core/utils/global.h"
#include "core/geometry/mesh3d.h"

namespace corecvs {


class MeshLoader
{
public:
    enum DumpFormat {
        FORMAT_PLY,
        FORMAT_STL,
        FORMAT_OBJ
    };

    static inline const char *getName(const DumpFormat &value)
    {
        switch (value)
        {
         case FORMAT_PLY : return "FORMAT_PLY"; break ;
         case FORMAT_STL : return "FORMAT_STL"; break ;
         case FORMAT_OBJ : return "FORMAT_OBJ"; break ;
        }
        return "Not in range";
    }


    bool trace;

    // Depricated
    //static bool endsWith(const std::string &fileName, const char *extention);

    MeshLoader();
    bool load(Mesh3D *mesh, const std::string &fileName);

    bool save(Mesh3D *mesh, const std::string &fileName);
    //bool save(Mesh3D *mesh, const DumpFormat &format);

    static std::string extentionList();

};

} // namespace corecvs

#endif // MESH_LOADER_H
