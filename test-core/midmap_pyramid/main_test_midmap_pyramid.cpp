/**
 * \file main_test_midmap_pyramid.cpp
 * \brief Add Comment Here
 *
 * \date Feb 25, 2010
 * \author alexander
 */

#include <stdlib.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/bufferFactory.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/mipmapPyramid.h"

#include <iostream>
#include <math.h>

#include "core/fileformats/bmpLoader.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/mesh3d.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"

#include <core/cameracalibration/calibrationDrawHelpers.h>

using namespace corecvs;

TEST(MidmapPyramid, testFunction)
{
    G12Buffer *buffer = BufferFactory::getInstance()->loadG12Bitmap("chess.bmp");
    if (buffer == nullptr)
    {
        cout << "Could not open test image" << endl;
        return;
    }
    CORE_ASSERT_TRUE(buffer != NULL, "missed MidmapPyramid test data");

    int numLevels = 8;
    AbstractMipmapPyramid<G12Buffer> *pyr = new AbstractMipmapPyramid<G12Buffer>(buffer, numLevels, 1.2f);
    for (int i = 0; i < numLevels; i++)
    {
        char outName[256];
        snprintf2buf(outName, "pyr%d.bmp", i);

        G12Buffer *level = pyr->levels[i];
        RGB24Buffer levelSave(level);
        BMPLoader().save(string(outName), &levelSave);
        delete level;
    }
    delete_safe(pyr);
    delete buffer;
}

TEST(MidmapPyramid, testTexure)
{
    RGB24Buffer buffer(128, 128);
    RGBColor white = RGBColor::White();
    buffer.checkerBoard(16, white);

    //RGB24Buffer *buffLena = new RGB24Buffer(256,256);
    //buffLena = BufferFactory::getInstance()->loadRGB24Bitmap("lena.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(&buffer, "chess.bmp");

    int h = 900;
    int w = 900;
    RGB24Buffer *bufferpic = new RGB24Buffer(h, w, RGBColor::Red());

    ClassicRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), degToRad(50));
    renderer.modelviewMatrix = cam.getKMatrix();
    renderer.drawFaces = true;
    Mesh3DDecorated mesh;

    renderer.addTexture(&buffer, true);

    Vector3dd a01(-25.0, 10.0, -40.0);
    Vector3dd a02(-25.0, 0.0, -40.0);
    Vector3dd a03(-15.0, 0.0, -40.0);

    Vector3dd a11(-20.0, 10.0, -25.0);
    Vector3dd a12(-20.0, 0.0, -25.0);
    Vector3dd a13(-10.0, 0.0, -25.0);

    Vector3dd a21(-15.0, 10.0, 20.0);
    Vector3dd a22(-15.0, 0.0, 20.0);
    Vector3dd a23(-5.0, 0.0, 20.0);

    Vector3dd a31(-7.5, 10.0, 75.0);
    Vector3dd a32(-7.5, 0.0, 75.0);
    Vector3dd a33(3.5, 0.0, 75.0);

    Vector3dd a41(5.0, 10.0, 160.0);
    Vector3dd a42(5.0, 0.0, 160.0);
    Vector3dd a43(15.0, 0.0, 160.0);

    Vector3dd a51(25.0, 10.0, 320.0);
    Vector3dd a52(25.0, 0.0, 320.0);
    Vector3dd a53(35.0, 0.0, 320.0);

    Vector3dd a61(62.0, 10.0, 640.0);
    Vector3dd a62(62.0, 0.0, 640.0);
    Vector3dd a63(72.0, 0.0, 640.0);

    Vector3dd a71(110.0, 10.0, 999.0);
    Vector3dd a72(110.0, 0.0, 999.0);
    Vector3dd a73(120.0, 0.0, 999.0);  

    Vector3dd a81(170.0, 0.0, 750.0);
    Vector3dd a82(20.0, 5.0, -25.0);
    Vector3dd a83(20.0, -5.0, -25.0);
    
    Vector3dd a91(210.0, 10.0, 1700.0);
    Vector3dd a92(210.0, 0.0, 1700.0);
    Vector3dd a93(220.0, 0.0, 1700.0);

    Vector3dd a101(360.0, 10.0, 2700.0);
    Vector3dd a102(360.0, 0.0, 2700.0);
    Vector3dd a103(370.0, 0.0, 2700.0);

    mesh.mulTransform(Affine3DQ::Shift(0, 0, 100));
    mesh.addTriangle(a01,a02,a03); 
    mesh.addTriangle(a11,a12,a13);
    mesh.addTriangle(a21,a22,a23);
    mesh.addTriangle(a31,a32,a33);
    mesh.addTriangle(a41,a42,a43);
    mesh.addTriangle(a51,a52,a53);
    mesh.addTriangle(a61,a62,a63);
    mesh.addTriangle(a71,a72,a73);
    mesh.addTriangle(a81,a82,a83);
    mesh.addTriangle(a91,a92,a93);
    mesh.addTriangle(a101,a102,a103);
    mesh.popTransform();

    Vector2dd b1(0.0, 1.0);
    Vector2dd b2(0.0, 0.0);
    Vector2dd b3(1.0, 0.0);

    Vector3dd a1(0.0, 0.0, 100.0);
    Vector3dd a2((-31.0/6.0), 0.0, -1.0);

    mesh.texId.resize(11);
    mesh.normalId.resize(11);
    mesh.textureCoords.resize(11);
    mesh.normalCoords.resize(11);


    for (int i = 0; i < 11; i++){
        mesh.texId[i][0] = 0;
        mesh.texId[i][1] = 1;
        mesh.texId[i][2] = 2;
        mesh.texId[i][3] = 0;

        mesh.normalId[i][0] = 0;
        mesh.normalId[i][1] = 1;
        mesh.normalId[i][2] = 2;

        mesh.textureCoords[mesh.texId[i][0]] = b1;
        mesh.textureCoords[mesh.texId[i][1]] = b2;
        mesh.textureCoords[mesh.texId[i][2]] = b3;

        mesh.normalCoords[mesh.normalId[i][0]] = a1;
        mesh.normalCoords[mesh.normalId[i][1]] = a1;
        mesh.normalCoords[mesh.normalId[i][2]] = a1;
    }

    mesh.normalCoords[mesh.normalId[8][0]] = a2;
    mesh.normalCoords[mesh.normalId[8][1]] = a2;
    mesh.normalCoords[mesh.normalId[8][2]] = a2;


    renderer.render(&mesh, bufferpic);

    BMPLoader().save("trianglesdraw_true2.bmp", bufferpic);
    renderer.dumpAllDebugs("meshdraw-");

    cout << buffer.elementBl(15.5, 15.5);
    CameraModel model;
    model.intrinsics.reset(cam.clone());
    model.setLocation(Affine3DQ::Identity());
    CalibrationDrawHelpers helper;
    helper.drawCamera(mesh, model, 0.5);
    mesh.dumpPLY("test.ply");

    delete_safe(bufferpic);
}


TEST(MidmapPyramid, testTexureDY)
{
    RGB24Buffer buffer(128, 128);
    RGBColor white = RGBColor::White();
    buffer.checkerBoard(16, white);

    int h = 100;
    int w = 100;
    RGB24Buffer *bufferpic = new RGB24Buffer(h, w, RGBColor::Red());

    ClassicRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), degToRad(50));
    renderer.modelviewMatrix = cam.getKMatrix();
    renderer.drawFaces = true;
    Mesh3DDecorated mesh;
    renderer.addTexture(&buffer, true);

    mesh.currentTexture = 0;
    mesh.switchTextures(true);
    mesh.switchNormals(true);

    mesh.addTriangleT(Vector3dd(-1, -1, 3), Vector2dd(0, 0),
                      Vector3dd( 1, -1, 3), Vector2dd(1, 0),
                      Vector3dd(-1,  1, 3), Vector2dd(0, 1));



    renderer.render(&mesh, bufferpic);
    SYNC_PRINT(("Render done\n"));
    BMPLoader().save("triangle.bmp", bufferpic);
    renderer.dumpAllDebugs("test");

    cout << buffer.elementBl(15.5, 15.5);
    CameraModel model;
    model.intrinsics.reset(cam.clone());
    model.setLocation(Affine3DQ::Identity());
    CalibrationDrawHelpers helper;
    helper.drawCamera(mesh, model, 0.5);
    mesh.dumpPLY("test2.ply");

    delete_safe(bufferpic);

}

