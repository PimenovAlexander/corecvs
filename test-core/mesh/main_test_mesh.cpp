/**
 * \file main_test_mesh.cpp
 * \brief This is the main file for the test mesh 
 *
 * \date мар 29, 2019
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/mesh/mesh3DDecorated.h"

using namespace std;
using namespace corecvs;

TEST(Mesh, testMeshAdd1)
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

TEST(Mesh, testRecomputeNormals)
{
    Mesh3DDecorated m1;
    m1.addIcoSphere(Vector3dd::Zero(), 10.0, 0);
    m1.switchNormals();
    m1.recomputeMeanNormals();
}


