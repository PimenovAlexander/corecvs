/**
 * \file main_test_renderer.cpp
 * \brief This is the main file for the test renderer
 *
 * \date апр. 11, 2016
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>

#include "gtest/gtest.h"
#include "core/utils/global.h"

#include "core/fileformats/bmpLoader.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/mesh3d.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/simpleRenderer.h"



using namespace std;

TEST(Renderer, inverseZ)
{
    int h = 200;
    int w = 200;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    SimpleRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), 50);
    renderer.modelviewMatrix = cam.getKMatrix();
    renderer.drawEdges = false;
    renderer.drawVertexes = false;

    Mesh3D mesh;

    mesh.addTriangle(Vector3dd(40,-40, -10), Vector3dd(40,  40,   -10), Vector3dd(40, -40, -4000));
    mesh.addTriangle(Vector3dd(40, 40, -10), Vector3dd(40, -40, -4000), Vector3dd(40,  40, -4000));

    //mesh.addSphere(Vector3dd(0, 0, -100), 20, 20);

    renderer.render(&mesh, buffer);

    mesh.dumpPLY("invz.ply");
    BMPLoader().save("invzdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("invzdraw-z.bmp", buffer);

    delete_safe(buffer);
}

TEST(Renderer, renderMesh)
{
    int h = 200;
    int w = 200;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    SimpleRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), 50);
    renderer.modelviewMatrix = cam.getKMatrix();
    renderer.drawEdges = false;
    renderer.drawVertexes = false;

    Mesh3D mesh;
    mesh.addIcoSphere(Vector3dd(0, 0, -200), 20, 4);
    //mesh.addSphere(Vector3dd(0, 0, -100), 20, 20);

    renderer.render(&mesh, buffer);

    BMPLoader().save("meshdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("meshdraw-z.bmp", buffer);

    delete_safe(buffer);

}

TEST(renderer, testrenderer)
{
    cout << "Starting test <renderer>" << endl;
    cout << "Test <renderer> PASSED" << endl;
}
