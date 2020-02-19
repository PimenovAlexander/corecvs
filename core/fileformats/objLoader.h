#ifndef OBJLOADER_H
#define OBJLOADER_H
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "utils/global.h"

#include "geometry/mesh/mesh3d.h"
#include "geometry/mesh/mesh3DDecorated.h"



#include "buffers/rgb24/rgb24Buffer.h"
namespace corecvs {

using std::vector;

class OBJLoader {
public:
    bool trace = true;


    OBJLoader();
    virtual ~OBJLoader();
    int loadOBJ(std::istream &input, Mesh3DDecorated &mesh);
    int loadMaterials(std::istream &input, vector<OBJMaterial> &materials, const std::string &path = "");
    int saveMaterials(const std::string &fileName, vector<OBJMaterial> &materials, const std::string &path = "");

    int loadOBJSimple(std::istream &input, Mesh3D &mesh);
    int saveOBJSimple(std::ostream &out  , Mesh3D &mesh);

    /** Not yet finished **/
    int saveObj(const std::string &fileName, Mesh3DDecorated &mesh);
};

} // namespace corecvs
/* EOF */


#endif // OBJLOADER_H
