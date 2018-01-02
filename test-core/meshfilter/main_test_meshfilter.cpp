#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"
#include "core/fileformats/meshLoader.h"
using namespace std;
using namespace corecvs;

TEST(FilterTest, DuplVertTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2

    mesh.vertexes.push_back(Vector3dd(1,3,3));  //3
    mesh.vertexes.push_back(Vector3dd(1,3,3)); //4

    mesh.faces.push_back(Vector3d32(0,1,3));
    mesh.faces.push_back(Vector3d32(0,2,4));

    ASSERT_EQ(mesh.vertexes.size(), 5);
    ASSERT_EQ(mesh.faces[1][2], 4);

    MeshFilter::removeDuplicatedVertices(mesh);

    ASSERT_EQ(mesh.vertexes.size(), 4);
    ASSERT_EQ(mesh.faces[1][2], 3);

}

// test no dupl vertices
TEST(FilterTest, DuplVertZeroTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2


    ASSERT_EQ(mesh.vertexes.size(), 3);

    MeshFilter::removeDuplicatedVertices(mesh);

    ASSERT_EQ(mesh.vertexes.size(), 3);

}

// test only 1 dupl face
TEST(FilterTest, DuplFacesTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2


    mesh.faces.push_back(Vector3d32(0,1,2));
    mesh.faces.push_back(Vector3d32(0,2,1));

    ASSERT_EQ(mesh.faces.size(), 2);

    MeshFilter::removeDuplicatedFaces(mesh);

    ASSERT_EQ(mesh.faces.size(), 1);

}

// test many dupl faces
TEST(FilterTest, DuplFacesManyTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2
    mesh.vertexes.push_back(Vector3dd(1,3,3)); //3
    mesh.vertexes.push_back(Vector3dd(1,3,4)); //4


    mesh.faces.push_back(Vector3d32(0,1,2));
    mesh.faces.push_back(Vector3d32(0,2,1));
    mesh.faces.push_back(Vector3d32(2,1,0));

    mesh.faces.push_back(Vector3d32(0,1,3));
    mesh.faces.push_back(Vector3d32(0,3,1));
    mesh.faces.push_back(Vector3d32(3,1,0));

    mesh.faces.push_back(Vector3d32(0,1,4));
    mesh.faces.push_back(Vector3d32(0,4,1));
    mesh.faces.push_back(Vector3d32(4,1,0));

    ASSERT_EQ(mesh.faces.size(), 9);

    MeshFilter::removeDuplicatedFaces(mesh);

    ASSERT_EQ(mesh.faces.size(), 3);

}

// test unreferenced vertexes
TEST(FilterTest,UnrefVertTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2
    mesh.vertexes.push_back(Vector3dd(1,3,4)); //3
    mesh.vertexes.push_back(Vector3dd(1,3,5)); //4
    mesh.vertexes.push_back(Vector3dd(1,4,5)); //5



    mesh.faces.push_back(Vector3d32(5,1,2));
    mesh.faces.push_back(Vector3d32(5,3,1));

    ASSERT_EQ(mesh.vertexes.size(), 6);

    MeshFilter::removeUnreferencedVertices(mesh);

    ASSERT_EQ(mesh.vertexes.size(), 4);
    ASSERT_EQ(mesh.faces[0][0],3);
}

// test no unreferenced vertices
TEST(FilterTest, UnrefVertZeroTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(0,0,0)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2


    mesh.faces.push_back(Vector3d32(0,1,2));

    ASSERT_EQ(mesh.vertexes.size(), 3);

    MeshFilter::removeUnreferencedVertices(mesh);

    ASSERT_EQ(mesh.vertexes.size(), 3);
}

// test all isolated pieces, ,min num of faces = 2
TEST(FilterTest, IsolPiecesAllTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,0,0)); //1
    mesh.vertexes.push_back(Vector3dd(0,1,0)); //2
    mesh.vertexes.push_back(Vector3dd(6,0,0)); //3
    mesh.vertexes.push_back(Vector3dd(1,2,0)); //4
    mesh.vertexes.push_back(Vector3dd(0,1,3)); //5
    mesh.vertexes.push_back(Vector3dd(5,0,0)); //6
    mesh.vertexes.push_back(Vector3dd(2,2,0)); //7
    mesh.vertexes.push_back(Vector3dd(1,1,3)); //8


    mesh.faces.push_back(Vector3d32(3,4,5));
    mesh.faces.push_back(Vector3d32(6,7,8));
    mesh.faces.push_back(Vector3d32(0,1,2));


    ASSERT_EQ(mesh.vertexes.size(), 9);
    ASSERT_EQ(mesh.faces.size(), 3);


    mesh.dumpPLY("iso-before.ply");
    MeshFilter::removeIsolatedPieces(mesh, 2);

    ASSERT_EQ(mesh.vertexes.size(), 0);
    ASSERT_EQ(mesh.faces.size(), 0);

}

// test 1 isolated piece, min num of faces = 2
TEST(FilterTest, IsolPiecesTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,0,0)); //1
    mesh.vertexes.push_back(Vector3dd(0,1,0)); //2
    mesh.vertexes.push_back(Vector3dd(6,0,0)); //3
    mesh.vertexes.push_back(Vector3dd(1,2,0)); //4
    mesh.vertexes.push_back(Vector3dd(0,1,3)); //5
    mesh.vertexes.push_back(Vector3dd(5,0,0)); //6
    mesh.vertexes.push_back(Vector3dd(2,2,0)); //7
    mesh.vertexes.push_back(Vector3dd(1,1,3)); //8


    mesh.faces.push_back(Vector3d32(3,4,5));
    mesh.faces.push_back(Vector3d32(3,4,2));

    mesh.faces.push_back(Vector3d32(6,7,8));


    ASSERT_EQ(mesh.faces.size(), 3);

    MeshFilter::removeIsolatedPieces(mesh, 2);

    ASSERT_EQ(mesh.faces.size(), 2);

}
// test no zeroArea faces
TEST(FilterTest, ZeroAreaFacesNoTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,2,1)); //1
    mesh.vertexes.push_back(Vector3dd(1,3,2)); //2

    mesh.faces.push_back(Vector3d32(0,1,2));

    ASSERT_EQ(mesh.faces.size(), 1);

    MeshFilter::removeZeroAreaFaces(mesh);

    ASSERT_EQ(mesh.faces.size(), 1);

}
// test all zeroArea faces
TEST(FilterTest, ZeroAreaFacesAllTest)
{
    Mesh3DDecorated mesh;
    mesh.vertexes.push_back(Vector3dd(1,1,1)); //0
    mesh.vertexes.push_back(Vector3dd(1,1,3)); //1
    mesh.vertexes.push_back(Vector3dd(1,1,2)); //2

    mesh.faces.push_back(Vector3d32(0,1,2));

    ASSERT_EQ(mesh.faces.size(), 1);

    MeshFilter::removeZeroAreaFaces(mesh);

    ASSERT_EQ(mesh.faces.size(), 0);

}
