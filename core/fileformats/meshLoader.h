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


#include "global.h"
#include "mesh3d.h"

namespace corecvs {


class MeshLoader
{
public:
    bool trace;

    static bool endsWith(const std::string &fileName, const char *extention);

    MeshLoader();
    bool load(Mesh3D *mesh, const std::string &fileName);

    bool save(Mesh3D *mesh, const std::string &fileName);

    static std::string extentionList();

};

} // namespace corecvs

#endif // MESH_LOADER_H
