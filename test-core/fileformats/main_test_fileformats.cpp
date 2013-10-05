/**
 * \file main_test_fileformats.cpp
 * \brief This is the main file for the test fileformats
 *
 * \date Aug 22, 2010
 * \author alexander
 * \ingroup autotest
 */

#include <global.h>
#include <sstream>
#include <iostream>
#include "rawLoader.h"
#include "g12Buffer.h"
#include "bmpLoader.h"
#include "ppmLoader.h"
#include "plyLoader.h"

using namespace std;
using namespace corecvs;

void testFileFormats ( void )
{
    /** Test case 1 */
    RAWLoader *rawLoader = new RAWLoader();
    G12Buffer *raw = rawLoader->load("data/testdata/32x32_12h_test_raw.raw");
    ASSERT_TRUE(raw != NULL, "RAW Image load failed");
    ASSERT_TRUE(raw->h == raw->w, "RAW Image sizes corrupted");
    ASSERT_TRUE(raw->verify(), "RAW Image verification failed");
    delete raw;
    delete rawLoader;


    /** Test case 2 */
    BMPLoader *bmpLoader = new BMPLoader();
    G12Buffer *bmp = bmpLoader->load("data/testdata/test_bmp.bmp");
    ASSERT_TRUE(bmp->h == bmp->w, "BMP Image sizes corrupted");
    ASSERT_TRUE(bmp != NULL, "BMP Image load failed");
    ASSERT_TRUE(bmp->verify(), "BMP Image verification failed");
    delete bmp;
    delete bmpLoader;

    /** Test case 3 */
    PPMLoader *ppmLoader = new PPMLoader();
    G12Buffer *ppm = ppmLoader->load("data/testdata/test_ppm.ppm");
    ASSERT_TRUE(ppm != NULL, "PPM Image load failed");
    ASSERT_TRUE(ppm->h == ppm->w, "PPM Image sizes corrupted");
    ASSERT_TRUE(ppm->verify(), "PPM Image verification failed");
    delete ppm;
    delete ppmLoader;


    /** Test case 2 */
    BMPLoader *bmpLoader1 = new BMPLoader();
    G12Buffer *bmp1 = bmpLoader1->load("data/calib-object.bmp");
    ASSERT_TRUE(bmp1 != NULL, "BMP Image load failed");
    ASSERT_TRUE(bmp1->verify(), "BMP Image verification failed");
    delete bmp1;
    delete bmpLoader1;
}

void testPlyLoader()
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

int main (int /*argC*/, char ** /*argV*/)
{
    testPlyLoader();
    cout << "PASSED" << endl;
    return 0;
}
