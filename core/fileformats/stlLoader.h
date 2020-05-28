#ifndef STLLOADER_H
#define STLLOADER_H
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

class STLLoader {

public:
    STLLoader();
    virtual ~STLLoader();
    int loadAsciiSTL (std::istream &input, Mesh3D &mesh);
    int loadBinarySTL(std::istream &input, Mesh3D &mesh);

    int saveAsciiSTL (std::ostream &out, Mesh3D &mesh);
    int saveBinarySTL(std::ostream &out, Mesh3D &mesh);

};

} // namespace corecvs
/* EOF */

#endif // STLLOADER_H
