#ifndef STLLOADER_H
#define STLLOADER_H
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "core/utils/global.h"

#include "core/geometry/mesh3d.h"

namespace corecvs {

using std::vector;

class STLLoader {

public:
    STLLoader();
    virtual ~STLLoader();
    int loadAsciiSTL (istream &input, Mesh3D &mesh);
    int loadBinarySTL(istream &input, Mesh3D &mesh);

    int saveAsciiSTL(ostream &out, Mesh3D &mesh);
    int saveBinarySTL(ostream &out, Mesh3D &mesh);

};

} // namespace corecvs
/* EOF */

#endif // STLLOADER_H
