#ifndef STLLOADER_H
#define STLLOADER_H
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "global.h"

#include "mesh3d.h"

namespace corecvs {

using std::vector;

class STLLoader {
public:
    STLLoader();
    virtual ~STLLoader();
    int loadAsciiSTL(istream &input, Mesh3D &mesh);
    int loadBinarySTL(istream &input, Mesh3D &mesh);

};

} // namespace corecvs
/* EOF */

#endif // STLLOADER_H
