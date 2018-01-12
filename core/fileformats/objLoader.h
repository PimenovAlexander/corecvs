#ifndef OBJLOADER_H
#define OBJLOADER_H
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "core/utils/global.h"

#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"


#include "core/buffers/rgb24/rgb24Buffer.h"
namespace corecvs {

using std::vector;

class OBJLoader {
public:
    bool trace = true;


    OBJLoader();
    virtual ~OBJLoader();
    int loadOBJ(istream &input, Mesh3DDecorated &mesh);
    int loadMaterials(istream &input, vector<OBJMaterial> &materials, const std::string &path = "");

    int loadOBJSimple(istream &input, Mesh3D &mesh);
    int saveOBJSimple(ostream &out, Mesh3D &mesh);
};

} // namespace corecvs
/* EOF */


#endif // OBJLOADER_H
