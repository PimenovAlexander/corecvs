#include <iostream>
#include <fstream>

#include <QApplication>
#include <QtCore>
#include "qtFileLoader.h"


#include "calibrationCamera.h"
#include "renderer/simpleRenderer.h"
#include "mesh3d.h"
#include "meshLoader.h"
#include "objLoader.h"
#include "rgb24Buffer.h"
#include "cameraModel.h"
#include "bmpLoader.h"

using namespace corecvs;

#if 0
int main(int argc, const char **argv)
{
    int h = 2000;
    int w = 2000;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    SimpleRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), 50);
    renderer.modelviewMatrix = cam.getKMatrix();
    renderer.drawEdges = false;
    renderer.drawVertexes = false;

    Mesh3D mesh;
    MeshLoader loader;
    loader.trace = true;

    for ( int  i = 1; i < argc; i++ )
    {
        loader.load(&mesh, argv[i]);
    }
    if (!mesh.hasColor) {
       mesh.switchColor();
       for (size_t i = 0;i < mesh.facesColor.size();i++)
       {
           mesh.facesColor[i] = RGBColor::rainbow1((double)i / mesh.facesColor.size());
       }
    }

    //mesh.addSphere(Vector3dd(0, 0, -100), 20, 20);
    mesh.transform(Matrix44::Shift(0, 50 , 400) * Matrix44::RotationX(degToRad(90.0)));

    mesh.dumpInfo(cout);
    SYNC_PRINT(("Starting render...\n"));

    renderer.render(&mesh, buffer);

    BMPLoader().save("meshdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("meshdraw-z.bmp", buffer);

    delete_safe(buffer);
    return 0;
}
#else

void prepareMesh(Mesh3DDecorated &mesh, RGB24Buffer *texture)
{
    mesh.switchColor(true);
    mesh.addIcoSphere(Vector3dd(0,0,0), 50, 2);
    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        Vector3dd vertex = mesh.vertexes[i];
        mesh.normalCoords.push_back(vertex.normalised());
        Vector3dd spherical = Vector3dd::toSpherical(vertex);
        double u = (spherical.x() + M_PI) / (2 * M_PI);
        double v = (spherical.y() + M_PI / 2) / (M_PI);

        mesh.textureCoords.push_back(Vector2dd(u, v));

        //mesh.normalCoords.push_back(Vector3dd(1.0, 0.5, 0.24).normalised());
        //mesh.textureCoords.push_back(Vector2dd(1.0, 0.5));
        cout << vertex << " -> " << mesh.textureCoords.back() << endl;
        mesh.vertexesColor[i] = RGBColor::rainbow1((double) i / mesh.vertexes.size());
    }

    for (size_t j = 0; j < mesh.faces.size(); j++)
    {
        mesh.normalId.push_back(mesh.faces[j]);
        mesh.texId.push_back(mesh.faces[j]);
        mesh.facesColor[j] = RGBColor::rainbow1((double) j / mesh.faces.size());

    }


    //mesh.addSphere(Vector3dd(0, 0, -100), 20, 20);
    mesh.transform(Matrix44::Shift(0, 0 , 400) * Matrix44::RotationX(degToRad(90.0)));
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    QTRGB24Loader::registerMyself();

    if (argc != 2 && argc != 3)
    {
        printf("Usage: softrender <obj basename>\n");
        exit(1);
    }

    std::string objName;
    std::string textureName;

    if (argc == 2) {
        objName     = std::string(argv[1]) + ".obj";
        textureName = std::string(argv[1]) + ".bmp";
    }

    if (argc == 3) {
        objName     = std::string(argv[1]);
        textureName = std::string(argv[2]);
    }

    printf("Will render <%s> with texture <%s>\n", objName.c_str(), textureName.c_str());

    int h = 2000;
    int w = 2000;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    ClassicRenderer renderer;
    PinholeCameraIntrinsics cam(Vector2dd(w,h), 50);
    renderer.modelviewMatrix = cam.getKMatrix();

    Mesh3DDecorated mesh;
    OBJLoader loader;
    loader.trace = true;

    //prepareMesh(mesh);
    std::ifstream file(objName, std::ifstream::in);
    loader.loadOBJ(file, mesh);

    mesh.transform(Matrix44::Shift(0, 0 , 3500) /** Matrix44::Scale(100)*/);

    RGB24Buffer *texture = BufferFactory::getInstance()->loadRGB24Bitmap(textureName);

/*
    int square = 64;
    RGB24Buffer *texture = new RGB24Buffer(512, 1024);
    for (int i = 0; i < texture->h; i++)
    {
        for (int j = 0; j < texture->w; j++)
        {
            bool color = ((i / square) % 2) ^ ((j / square) % 2);
            texture->element(i, j)  = (color ?  RGBColor::White() : RGBColor::Black());
        }
    }*/
    if (!texture) {
        SYNC_PRINT(("Could not load texture"));
    } else {
        SYNC_PRINT(("Texture: [%d x %d]\n", texture->w, texture->h));
    }


    renderer.textures.push_back(texture);

    mesh.dumpInfo(cout);
    SYNC_PRINT(("Starting render...\n"));

    renderer.render(&mesh, buffer);

    BMPLoader().save("meshdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("meshdraw-z.bmp", buffer);

    delete_safe(buffer);
    return 0;
}

#endif
