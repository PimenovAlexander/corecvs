#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "mesh3d.h"
#include "meshLoader.h"

using namespace std;
using namespace corecvs;

TEST(meshdraw, testmeshdraw_circle)
{
    cout << "Starting test <meshdraw_circle>" << endl;
    Mesh3D mesh;
    mesh.drawCircle(Vector3dd::Zero(), 10, 30);
    mesh.dumpPLY("mesh-circle.ply");

}

