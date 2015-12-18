#ifndef MESHLOADER_H
#define MESHLOADER_H
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
    MeshLoader();
    static bool load(Mesh3D *mesh, const std::string &fileName);

    static std::string extentionList();

};

} // namespace corecvs

#endif // MESHLOADER_H
