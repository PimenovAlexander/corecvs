/**
 * \file main_test_bsptree.cpp
 * \brief This is the main file for the test bsptree 
 *
 * \date май 14, 2018
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <core/geometry/mesh3d.h>
#include <core/geometry/raytrace/bspTree.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/automotive/simulation/testSceneSimulator.h"

using namespace std;
using namespace corecvs;

TEST(BSPTree, testBSPTree)
{
    cout << "Starting test <bsptree>" << endl;

    Mesh3D mesh;
    mesh.addIcoSphere(Vector3dd::Zero(), 10, 0);
    BSPTreeNode<BSPVector3ddPointer> node;

    node.submesh.reserve(mesh.vertexes.size());
    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        node.submesh.push_back(BSPVector3ddPointer(&mesh.vertexes[i]));
    }

    PreciseTimer timer = PreciseTimer::currentTime();
    node.subdivide();
    SYNC_PRINT(("BSP build %lf s elapsed\n" ,timer.msecsToNow() / 1000.0));

    SYNC_PRINT(("Mesh tree   size is   %d\n", node.childCount()));
    SYNC_PRINT(("Mesh tree   points %d\n", node.elementCount()));
    SYNC_PRINT(("Mesh inital points %d\n", (int)mesh.vertexes.size()));

    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        Vector3dd *point = &mesh.vertexes[i];
        BSPVector3ddPointer request(point);

        BSPVector3ddPointer *found = node.findClosest(request);
        cout << "Request " <<  request.p << " (" << *(request.p) << ")  " << found->p << " (" << *(found->p)<< ")"<< std::endl;

        CORE_ASSERT_TRUE(point == found->p, "Wrong point");
    }


    cout << "Test <bsptree> PASSED" << endl;
}


bool verifyTree (BSPTreeNode<BSPVector3ddPointer> *node)
{

    return false;
}

TEST(BSPTree, testBSPTree1)
{
    cout << "Starting test <bsptree>" << endl;

    const int SIZE = 200;

    Mesh3D mesh;
    TestSceneSimulator::unitCube(SIZE + 1, mesh.vertexes);
    mesh.transform(Matrix44::Scale(SIZE));

    BSPTreeNode<BSPVector3ddPointer> node;

    node.submesh.reserve(mesh.vertexes.size());
    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        node.submesh.push_back(BSPVector3ddPointer(&mesh.vertexes[i]));
    }

    PreciseTimer timer = PreciseTimer::currentTime();
    node.subdivide();
    SYNC_PRINT(("BSP build %lf s elapsed\n" ,timer.msecsToNow() / 1000.0));

    SYNC_PRINT(("Mesh tree   size is   %d\n", node.childCount()));
    SYNC_PRINT(("Mesh tree   points %d\n", node.elementCount()));
    SYNC_PRINT(("Mesh inital points %d\n", (int)mesh.vertexes.size()));



    timer = PreciseTimer::currentTime();

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(-0.4, 0.4);

    /* Separate test */
    cout << "Make some checks:" << endl;

    Vector3dd point(-2.615, -2.819, 3.7415);
    BSPVector3ddPointer request(&point);
    BSPVector3ddPointer *found = node.findClosest(request);
    cout << "Request " << request.p << " (" << *(request.p) << ")  " << found->p << " (" << *(found->p)<< ")"<< std::endl;


    /* Full cycle */

    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {       
        Vector3dd noise = Vector3dd(runif(rng), runif(rng), runif(rng));
        //cout << noise << endl;
        Vector3dd point = mesh.vertexes[i] + noise;
        BSPVector3ddPointer request(&point);

        BSPVector3ddPointer *found = node.findClosest(request);
        //cout << "Request " << mesh.vertexes[i] << " - " << request.p << " (" << *(request.p) << ")  " << found->p << " (" << *(found->p)<< ")"<< std::endl;

        CORE_ASSERT_TRUE_P
                (&mesh.vertexes[i] == found->p, ("Wrong point found (%lf %lf %lf) expected (%lf %lf %lf)",
                        found->p->x()       , found->p->y()       , found->p->z()       ,
                        mesh.vertexes[i].x(), mesh.vertexes[i].y(), mesh.vertexes[i].z()
                 ));
    }
    SYNC_PRINT(("BSP search %lf s elapsed\n" ,timer.msecsToNow() / 1000.0));

    cout << "Test <bsptree> PASSED" << endl;
}

TEST(BSPTree, testBSPTree2)
{
    Mesh3D mesh;
    TestSceneSimulator::unitCube(20, mesh.vertexes);
    mesh.transform(Matrix44::Scale(20.0));

    BSPTreeNode<Vector3dd> tree;
    tree.submesh.resize(mesh.vertexes.size());
    for (Vector3dd p : mesh.vertexes)
    {
        tree.submesh.push_back(p);
    }

    SYNC_PRINT(("creating BSP..."));
    tree.subdivide();
    SYNC_PRINT(("done\n"));

    Vector3dd request = Vector3dd::Zero();
    Vector3dd *target = tree.findClosest(request);

    cout << *target << endl;

}

