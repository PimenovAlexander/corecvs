/**
 * \file main_test_meshdraw.cpp
 * \brief This is the main file for the test meshdraw
 *
 * \date окт. 11, 2015
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/geometry/mesh/meshCache.h"


using namespace corecvs;

TEST(Meshcache, testMeshCache)
{
    cout << "Starting test <testMeshCache>" << endl;
    Mesh3DDecorated mesh;

    mesh.currentTexture = 0;
    mesh.switchTextures();

    mesh.materials.resize(2);
    /* */

    mesh.addTriangleT(
        Vector3dd(0, 0, 0), Vector2dd(0, 0),
        Vector3dd(1, 0, 0), Vector2dd(1, 0),
        Vector3dd(0, 1, 0), Vector2dd(0, 1)
    );

    mesh.currentTexture = 1;
    mesh.addTriangleT(
        Vector3dd(0, 0, 1), Vector2dd(0, 0),
        Vector3dd(1, 0, 1), Vector2dd(1, 0),
        Vector3dd(0, 1, 1), Vector2dd(0, 1)
    );


    cout << "Additional data" << endl;
    for (size_t faceNum = 0; faceNum < mesh.faces.size(); faceNum++)
    {
         cout << mesh.texId[faceNum] << endl;
    }

    cout << "Inital mesh" << endl;
    mesh.dumpInfo();
    cout << endl;

    SceneShadedOpenGLCache cache;
    Draw3dParameters params;
    cache.cache(&mesh, params);

    cout << cache << endl;
}
