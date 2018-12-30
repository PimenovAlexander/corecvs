#include <iostream>
#include <fstream>

#include <QApplication>
#include <QtCore>
#include "qtFileLoader.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/objLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/utils.h"

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

void prepareMesh(Mesh3DDecorated &mesh, RGB24Buffer * /*texture*/)
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
        Vector4d32 texIdAndName(mesh.faces[j], 0);

        mesh.normalId.push_back(mesh.faces[j]);
        mesh.texId   .push_back(texIdAndName);
        mesh.facesColor[j] = RGBColor::rainbow1((double) j / mesh.faces.size());

    }


    //mesh.addSphere(Vector3dd(0, 0, -100), 20, 20);
//    mesh.transform(Matrix44::Shift(0, 0 , 400) * Matrix44::RotationX(degToRad(90.0)));
}

int main(int argc, char **argv)
{
    QApplication a(argc, argv);

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    QTRGB24Loader::registerMyself();

    BufferFactory::getInstance()->printCaps();


    if (argc != 2 && argc != 3 && argc != 4)
    {
        printf("Usage: \n"
               "  softrender <obj basename> \n"
               "  softrender <obj basename> <camera.json | camera.txt> \n");

        exit(1);
    }

    std::string objName;

    double fx, fy, cx, cy, w = 1000, h = 1000;
    if (argc >= 2) {
        objName     = std::string(argv[1]);
    }

    PinholeCameraIntrinsics cam(Vector2dd(w,h), degToRad(50));
    Affine3DQ pose;
    /*if (argc >= 3) {
		std::cout << "loading cam" << std::endl;
		std::ifstream cams;
		cams.open(argv[2]);
		std::string tag;
		cams >> tag >> fx >> fy >> cx >> cy >> w >> h;
		cam.setSize(Vector2dd(w, h));
		cam.setPrincipal(Vector2dd(cx, cy));
		cam.setFocal(Vector2dd(fx, fy));
    }*/

    ClassicRenderer renderer;

    Mesh3DDecorated *mesh = new Mesh3DDecorated();
    OBJLoader objLoader;

    /** Load Materials **/
    std::string mtlFile = objName.substr(0, objName.length() - 4) + ".mtl";
    std::ifstream materialFile;
    materialFile.open(mtlFile, std::ios::in);
    if (materialFile.good())
    {
        objLoader.loadMaterials(materialFile, mesh->materials, corecvs::HelperUtils::getDirectory(mtlFile));

        cout << "Loaded materials: " << mesh->materials.size() << std::endl;
    } else {
        cout << "Unable to load material from <" << mtlFile << ">" << std::endl;
    }
    materialFile.close();



    /** Load actual data **/
    SYNC_PRINT(("Starting actual data loading\n"));
    std::ifstream file;
    file.open(objName, std::ios::in);
    objLoader.loadOBJ(file, *mesh);
    file.close();


    for(size_t t = 0; t < mesh->materials.size(); t++)
    {
        renderer.addTexture(mesh->materials[t].tex[OBJMaterial::TEX_DIFFUSE], true);
    }

    SYNC_PRINT(("Loaded and added textures\n"));


	if (argc >= 4) {
		std::cout << "loading pose" << std::endl;
		std::ifstream poses;
		poses.open(argv[3]);
		int n;
		std::string tag;
		poses >> n >> tag;

		poses >> pose.shift.x() >> pose.shift.y() >> pose.shift.z() >> pose.rotor.x() >> pose.rotor.y() >> pose.rotor.z() >> pose.rotor.t();
    }
    
    printf("Will render <%s>\n", objName.c_str());


    pose = Affine3DQ::RotationX(degToRad(-25))
         * Affine3DQ::RotationY(degToRad(150))
         * Affine3DQ::RotationZ(degToRad(180))
         * Affine3DQ::Shift(0, 0, -2000);

    SYNC_PRINT(("Starting render...\n"));
    cout << "Camera:" << cam << endl;
    cout << "Position:" << pose << endl;


    renderer.modelviewMatrix = cam.getKMatrix() * Matrix44(pose.inverted());
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    renderer.render(mesh, buffer);


    BMPLoader().save("meshdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("meshdraw-z.bmp", buffer);
    renderer.dumpAllDebugs("meshdraw-");


    delete_safe(buffer);

    return 0;
}

#endif
