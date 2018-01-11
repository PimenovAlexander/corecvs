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
#include "core/geometry/mesh3DDecorated.h"
#include "core/fileformats/meshLoader.h"

using namespace std;
using namespace corecvs;


TEST(meshdraw, testmeshdraw)
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

}



TEST(meshdraw, normalGenerate)
{
    cout << "Starting test <meshdraw>. This is a manual test" << endl;
    Mesh3DDecorated mesh;
    mesh.switchColor();

    mesh.setColor(RGBColor::Red());
    mesh.addCylinder(Vector3dd::Zero(), 10, 10, 3);
    mesh.recomputeMeanNormals();

    for (size_t f = 0; f <  mesh.normalId.size(); f++)
    {
        Vector3d32  ids = mesh.normalId[f];
        Triangle3dd face = mesh.getFaceAsTrinagle(f);

        mesh.setColor(RGBColor::Green());
        mesh.addLine(face.p1(), face.p1() + mesh.normalCoords[ids.x()]);
        mesh.addLine(face.p2(), face.p2() + mesh.normalCoords[ids.y()]);
        mesh.addLine(face.p3(), face.p3() + mesh.normalCoords[ids.z()]);
    }

    mesh.dumpPLY("mesh-normals.ply");
}
