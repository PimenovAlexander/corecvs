/**
 * \file main_test_fileformats.cpp
 * \brief This is the main file for the test fileformats
 *
 * \date Aug 22, 2010
 * \author alexander
 * \ingroup autotest
 */
#include <sstream>
#include <iostream>
#include "gtest/gtest.h"

#include <global.h>
#include "rawLoader.h"
#include "g12Buffer.h"
#include "bmpLoader.h"
#include "ppmLoader.h"
#include "plyLoader.h"

using namespace std;
using namespace corecvs;

TEST(FileFormats, testFileFormats)
{
    /** Test case 1 */
    RAWLoader *rawLoader = new RAWLoader();
    G12Buffer *raw = rawLoader->load("data/testdata/32x32_12h_test_raw.raw");
    CORE_ASSERT_TRUE(raw != NULL, "RAW Image load failed");
    CORE_ASSERT_TRUE(raw->h == raw->w, "RAW Image sizes corrupted");
    CORE_ASSERT_TRUE(raw->verify(), "RAW Image verification failed");
    delete_safe(raw);
    delete_safe(rawLoader);

    /** Test case 2 */
    BMPLoader *bmpLoader = new BMPLoader();
    G12Buffer *bmp = bmpLoader->load("data/testdata/test_bmp.bmp");
    CORE_ASSERT_TRUE(bmp->h == bmp->w, "BMP Image sizes corrupted");
    CORE_ASSERT_TRUE(bmp != NULL, "BMP Image load failed");
    CORE_ASSERT_TRUE(bmp->verify(), "BMP Image verification failed");
    delete_safe(bmp);
    delete_safe(bmpLoader);

    /** Test case 3 */
    PPMLoader *ppmLoader = new PPMLoader();
    G12Buffer *ppm = ppmLoader->load("data/testdata/test_ppm.ppm");
    CORE_ASSERT_TRUE(ppm != NULL, "PPM Image load failed");
    CORE_ASSERT_TRUE(ppm->h == ppm->w, "PPM Image sizes corrupted");
    CORE_ASSERT_TRUE(ppm->verify(), "PPM Image verification failed");
    delete_safe(ppm);
    delete_safe(ppmLoader);

#if NONFREE
    /** Test case 4 */
    PPMLoader *metappmLoader = new PPMLoader();
    MetaData *metadata = new MetaData;
    G12Buffer *metappm = metappmLoader->load("data/testdata/test_pgm_metadata.pgm", metadata);
    CORE_ASSERT_TRUE(metappm != NULL, "PGM with Metadata Image load failed");
    CORE_ASSERT_TRUE(metappm->h == metappm->w, "PGM with Metadata Image sizes corrupted");
    CORE_ASSERT_TRUE(metappm->verify(), "PGM with Metadata Image verification failed");
    CORE_ASSERT_TRUE(metadata->at("hello_world")[0] == 42, "PGM Metadata read failed");
    delete_safe(metappm);
    delete_safe(metappmLoader);
    delete_safe(metadata);
#endif

    /** Test case 5 */
    BMPLoader *bmpLoader1 = new BMPLoader();
    G12Buffer *bmp1 = bmpLoader1->load("data/calib-object.bmp");
    CORE_ASSERT_TRUE(bmp1 != NULL, "BMP Image load failed");
    CORE_ASSERT_TRUE(bmp1->verify(), "BMP Image verification failed");
    delete_safe(bmp1);
    delete_safe(bmpLoader1);
}

TEST(FileFormats, testPlyLoader)
{
    const char *tests[4];
    tests[0] = "sdvfas";
    tests[1] =
        "ply\n"
        "format binary 2.0\n";

    tests[2] =
        "ply\n"
        "comment Blah Blah...\n"
        "format ascii 1.0\n";

    tests[3] =
        "ply\n"
        "format ascii 1.0\n"
        "comment made by Greg Turk\n"
        "comment this file is a cube\n"
        "element vertex 8\n"
        "property float x\n"
        "property float y\n"
        "property float z\n"
        "element face 6\n"
        "property list uchar int vertex_index\n"
        "end_header\n"
        "0 0 0\n"
        "0 0 1\n"
        "0 1 1\n"
        "0 1 0\n"
        "1 0 0\n"
        "1 0 1\n"
        "1 1 1\n"
        "1 1 0\n"
        "3 0 1 2 3\n"
        "3 7 6 5 4\n"
        "3 0 4 5 1\n"
        "3 1 5 6 2\n"
        "3 2 6 7 3\n"
        "3 3 7 4 0\n";

    /*    "4 0 1 2 3\n"
        "4 7 6 5 4\n"
        "4 0 4 5 1\n"
        "4 1 5 6 2\n"
        "4 2 6 7 3\n"
        "4 3 7 4 0\n";*/

    for (unsigned i = 0; i < CORE_COUNT_OF(tests); i++)
    {
        Mesh3D mesh;
        PLYLoader loader;
        std::string str(tests[i]);
        std::istringstream stream(str);
        int result = loader.loadPLY(stream, mesh);
        printf("Test case %d result: %d\n", i, result);
    }
}
