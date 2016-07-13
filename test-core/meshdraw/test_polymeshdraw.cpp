
#include <iostream>
#include "gtest/gtest.h"

#include "global.h"
#include "mesh3d.h"
#include "meshLoader.h"
#include "polylinemesh.h"

using namespace std;
using namespace corecvs;


TEST(meshdraw, testpolylinemesh)
{
    cout << "Starting test <testpolylinemesh>. This is a manual test" << endl;

    vector<Vector3dd> v;
    polylineMesh plm;
    polyLine pl;
    srand(time(NULL));
    for(int i = 0; i < 100; i++)
    {
        for(int k = 0; k < 100; k++)
        {
                v.push_back(Vector3dd(k, i, i*i / 100.0 - k*k / 100.0));
        }
        pl = polyLine(v);
        plm.addPolyline(pl);
        v.clear();
    }

    plm.mesh.dumpPLY("surface.ply");

}

