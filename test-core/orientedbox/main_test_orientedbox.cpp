/**
 * \file main_test_orientedbox.cpp
 * \brief This is the main file for the test orientedbox 
 *
 * \date май 28, 2018
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <core/geometry/mesh3d.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/orientedBox.h"


using namespace std;
using namespace corecvs;


void dumpToMesh (OrientedBox &box, Mesh3D &mesh)
{
    mesh.setColor(RGBColor::Cyan());

    for (int i = 0; i < 200; i++)
        for (int j = 0; j < 200; j++)
            for (int k = 0; k < 200; k++)
            {
                Vector3dd T = Vector3dd(i - 100, j - 100, k - 100) / 10.0;
                T += box.position;
                if (box.isInside(T))
                {
                    mesh.addPoint(T);
                }
            }

    mesh.setColor(RGBColor::Yellow());
    for (Vector3dd &p : box.getPoints())
    {
        mesh.addPoint(p);
    }
    mesh.setColor(RGBColor::Green());
    mesh.addOOB(box, true);


}

TEST(OrientedBox, testOrientedBoxFromAligned)
{
    cout << "Starting test <orientedbox>" << endl;
    AxisAlignedBox3d A(Vector3dd(-1, -2, -3), Vector3dd(1, 2, 3));
    OrientedBox box(A);

    Mesh3D mesh;
    mesh.switchColor();
    dumpToMesh(box, mesh);

    box.transform(Matrix44::Shift(10,10,10) * Matrix44::RotationZ(degToRad(45)));

    dumpToMesh(box, mesh);
    mesh.dumpPLY("oob.ply");

    cout << "Test <orientedbox> PASSED" << endl;
}

TEST(OrientedBox, testOrientedBoxFromStatistics)
{

}


TEST(OrientedBox, testOrientedBoxIntersection)
{
    AxisAlignedBox3d A(Vector3dd(-1, -2, -3), Vector3dd(1, 2, 3));
    OrientedBox box(A);

    {
        OrientedBox box1 = box;
        CORE_ASSERT_TRUE(box1.hasIntersection(box), "Self intersection failed");
    }

    {
        OrientedBox box1 = box.transformed(Matrix44::Shift(1,0,0));
        CORE_ASSERT_TRUE(box1.hasIntersection(box), "Shift intersection failed");
    }

    {
        OrientedBox box1 = box.transformed(Matrix44::Shift(2.01,0,0));
        CORE_ASSERT_TRUE(!box1.hasIntersection(box), "Far shift intersection failed");
    }

}
