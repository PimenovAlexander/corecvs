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
#include "core/geometry/mesh3d.h"
#include "core/fileformats/meshLoader.h"
#include "core/geometry/mesh3DDecorated.h"

using namespace corecvs;

TEST(Meshdraw, testMeshDraw)
{
    cout << "Starting test <meshdraw>. This is a manual test" << endl;
    Mesh3D mesh;
    mesh.switchColor();
    mesh.addOrts(10, true);
    mesh.dumpPLY("mesh-orts.ply");

    mesh.clear();
    mesh.addCylinder(Vector3dd::Zero(), 10, 10, 3);
    mesh.addCylinder(Vector3dd::OrtZ() * 20, 10, 10, 30);
    mesh.dumpPLY("mesh-cylinder.ply");

    mesh.clear();
    mesh.addIcoSphere(Vector3dd(0), 2, 0);
    MeshLoader loader;
    loader.save(&mesh, "mesh-icosphere.stl");
    loader.save(&mesh, "mesh-icosphere.ply");
    loader.save(&mesh, "mesh-icosphere.obj");

}

TEST(Meshdraw, testMeshDodecahedron)
{
    cout << "Starting test <meshdraw>. This is a manual test" << endl;
    Mesh3D mesh;
    mesh.addDodecahedron(Vector3dd(0), 2);
    MeshLoader loader;
    mesh.dumpPLY("mesh-dodecahedron.ply");

}

TEST(Meshdraw, testMeshScene)
{
    cout << "Starting test <meshdraw>. This is a manual test" << endl;
    Mesh3D mesh;
    mesh.fillTestScene();
    mesh.dumpPLY("mesh-example.ply");

    Mesh3DDecorated mesh1;
    mesh1.fillTestScene();
    mesh1.dumpPLY("mesh-example1.ply");
}

TEST(Meshdraw, testMeshAdd1)
{
    {
        Mesh3DDecorated m1;
        m1.addIcoSphere(Vector3dd::Zero(), 10.0, 0);
        m1.dumpInfo();

        CORE_ASSERT_TRUE(m1.vertexes.size() == 12, "Ordinary Ico fail");
        CORE_ASSERT_TRUE(m1.faces.size()    == 20, "Ordinary Ico fail");
    }

    {
        Mesh3DDecorated m1;
        m1.switchNormals();
        m1.addIcoSphere(Vector3dd::Zero(), 10.0, 0);
        m1.dumpInfo();

        CORE_ASSERT_TRUE(m1.vertexes.size() == 12, "Ordinary Ico fail");
        CORE_ASSERT_TRUE(m1.faces.size()    == 20, "Ordinary Ico fail");
        CORE_ASSERT_TRUE(m1.normalId.size() == 20, "Ordinary Ico fail");
    }

    {
        Mesh3DDecorated m1;
        m1.switchNormals();
        m1.addIcoSphere(Vector3dd::Zero(), 10.0, 0);
        m1.dumpInfo();

        Mesh3DDecorated m2;
        m2.switchNormals();
        m2.addIcoSphere(Vector3dd::Zero(), 10.0, 0);
        m2.dumpInfo();

        /* Merged */
        m1.add(m2);
        m1.dumpInfo();

        CORE_ASSERT_TRUE(m1.verify(), "Verify failed");
        CORE_ASSERT_TRUE(m1.vertexes.size() == 24, "Ordinary Ico fail");
        CORE_ASSERT_TRUE(m1.faces.size()    == 40, "Ordinary Ico fail");
        CORE_ASSERT_TRUE(m1.normalId.size() == 40, "Ordinary Ico fail");

    }

}

