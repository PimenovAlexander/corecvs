/**
 * \file main_test_midmap_pyramid.cpp
 * \brief Add Comment Here
 *
 * \date Feb 25, 2010
 * \author alexander
 */

#include <stdlib.h>
#include "gtest/gtest.h"

#include "global.h"

#include "g12Buffer.h"
#include "bufferFactory.h"
#include "bmpLoader.h"
#include "mipmapPyramid.h"

using namespace corecvs;

TEST(MidmapPyramid, testFunction)
{
    G12Buffer *buffer = BufferFactory::getInstance()->loadG12Bitmap("data/pair/image0001_c0.pgm");

    CORE_ASSERT_TRUE(buffer != NULL, "missed MidmapPyramid test data");

    int numLevels = 8;
    AbstractMipmapPyramid<G12Buffer> *pyr = new AbstractMipmapPyramid<G12Buffer>(buffer, numLevels, 1.2f);
    for (int i = 0; i < numLevels; i++)
    {
        char outName[256];
        snprintf2buf(outName, "pyr%d.bmp", i);

        G12Buffer *level = pyr->levels[i];
        RGB24Buffer levelSave(level);
        BMPLoader().save(string(outName), &levelSave);
        delete level;
    }
    delete_safe(pyr);
    delete buffer;
}
