/**
 * \file main_test_midmap_pyramid.cpp
 * \brief Add Comment Here
 *
 * \date Feb 25, 2010
 * \author alexander
 */

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <cstdio>

#include "global.h"

#include "g12Buffer.h"
#include "bufferFactory.h"
#include "bmpLoader.h"
#include "gaussian.h"
#include "mipmapPyramid.h"

using namespace corecvs;

int main ( int /*argC*/, char * /*argV*/[])
{
    G12Buffer *buffer = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");

    int numLevels = 8;
    AbstractMipmapPyramid<G12Buffer> *pyr = new AbstractMipmapPyramid<G12Buffer>(buffer, numLevels, 1.2f);
    for (int i = 0; i < numLevels; i++)
    {
        char outName[256];
        snprintf2buf(outName, "pyr%d.bmp", i);

        G12Buffer *level = pyr->levels[i];
        RGB24Buffer levelSave(level);
        BMPLoader().save(string(outName), &levelSave);
    }
    return 0;
}
