#ifndef OBJLOADER_H
#define OBJLOADER_H
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "global.h"

#include "mesh3d.h"
#include "mesh3DDecorated.h"

namespace corecvs {

using std::vector;

class OBJLoader {
public:
    bool trace = true;


    OBJLoader();
    virtual ~OBJLoader();
    int loadOBJ(istream &input, Mesh3DDecorated &mesh);

    int loadOBJSimple(istream &input, Mesh3D &mesh);
    int saveOBJSimple(ostream &out, Mesh3D &mesh);
};

} // namespace corecvs
/* EOF */


#endif // OBJLOADER_H
