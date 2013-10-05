#pragma once
/**
 * \file plyLoader.h
 *
 * \date Nov 13, 2012
 **/

#include <iostream>
#include <vector>

#include "global.h"

#include "mesh3d.h"


using std::vector;

class PLYLoader {
public:
    PLYLoader();
    int loadPLY(istream &input, Mesh3D &mesh);
    virtual ~PLYLoader();
};

/* EOF */
