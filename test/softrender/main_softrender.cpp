#include <iostream>
#include <fstream>

#include <core/reflection/commandLineSetter.h>

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#include "core/cameracalibration/cameraModel.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/objLoader.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/utils/utils.h"
#include "core/filesystem/folderScanner.h"
#include "buffers/bufferFactory.h"

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
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    BufferFactory::getInstance()->printCaps();


    if (argc != 2 && argc != 3 && argc != 4)
    {
        printf("Usage: \n"
               "  softrender <obj basename> \n"
               "  softrender <obj basename> <camera.json | camera.txt> \n"
               "  Parameters --fov=50 --w=1000 --h=1000\n"
               "  --lookFrom.x=30 --lookFrom.y=30 --lookFrom.z=30\n"
               "  --lookAt.x=0  --lookAt.y=0  --lookAt.z=0 \n"

               );

        exit(1);
    }

    std::string objName;

    if (argc >= 2) {
        objName     = std::string(argv[1]);
    }

    CommandLineSetter line(argc, argv);

    double fov = degToRad(50);
    double w = 1000;
    double h = 1000;

    fov = degToRad(line.getDouble("fov", radToDeg(fov)));
    w   = line.getDouble("w", 1000);
    h   = line.getDouble("h", 1000);

    PinholeCameraIntrinsics cam(Vector2dd(w,h), degToRad(50));

    Affine3DQ pose = Affine3DQ::Identity();

    if (line.hasOption("lookFrom"))
    {
        Vector3dd lookFrom = Vector3dd::Zero();
        Vector3dd lookAt   = Vector3dd::Zero();
        line.visit(lookFrom, "lookFrom");
        line.visit(lookAt  , "lookAt");

        pose.shift = lookFrom;

        Vector3dd zDir = lookAt - lookFrom;
        zDir.normalise();
        Vector3dd xDir = Vector3dd(-zDir.y(), zDir.x(), 0.0);
        xDir.normalise();
        Vector3dd yDir = xDir ^ zDir;
        yDir.normalise();

        pose.rotor = Quaternion::FromMatrix(Matrix33::FromRows(xDir, yDir, zDir));

    }

    if (line.hasOption("posefile")) {
        std::string posefile = line.getString("posefile", "in.txt");
        std::cout << "loading pose from <" << posefile << ">"<< std::endl;
        std::ifstream poses;
        poses.open(posefile);
        int n;
        std::string tag;
        poses >> n >> tag;
        poses >> pose.shift.x() >> pose.shift.y() >> pose.shift.z() >> pose.rotor.x() >> pose.rotor.y() >> pose.rotor.z() >> pose.rotor.t();
    }

    if (line.hasOption("preset")) {
        int preset = line.getInt("preset", 0);
        cout << "Would use preset position" << preset << endl;

        switch (preset) {
        case 0: {
            pose = Affine3DQ::RotationX(degToRad(-25))
                 * Affine3DQ::RotationY(degToRad(150))
                 * Affine3DQ::RotationZ(degToRad(180))
                 * Affine3DQ::Shift(0, 0, -2000);
            }
            break;
        default:
            break;
        }
    }

    ClassicRenderer renderer;

    Mesh3DDecorated *mesh = new Mesh3DDecorated();

    /** Load actual data **/
    if (corecvs::HelperUtils::endsWith(objName, ".obj"))
    {
        OBJLoader objLoader;

        /** Load Materials **/
        std::string mtlFile = objName.substr(0, objName.length() - 4) + ".mtl";
        std::ifstream materialFile;
        materialFile.open(mtlFile, std::ios::in);
        if (materialFile.good())
        {
            objLoader.loadMaterials(materialFile, mesh->materials, FolderScanner::getDirectory(mtlFile));
            cout << "Loaded materials: " << mesh->materials.size() << std::endl;
        } else {
            cout << "Unable to load material from <" << mtlFile << ">" << std::endl;
        }
        materialFile.close();

        SYNC_PRINT(("Starting actual data loading\n"));
        std::ifstream file;
        file.open(objName, std::ios::in);
        objLoader.loadOBJ(file, *mesh);
        file.close();

        for(size_t t = 0; t < mesh->materials.size(); t++)
        {
            renderer.addTexture(mesh->materials[t].tex[OBJMaterial::TEX_DIFFUSE], false);
        }

        SYNC_PRINT(("Loaded and added textures\n"));
    } else {

        MeshLoader loader;
        if (!loader.load(mesh, objName)) {
            SYNC_PRINT(("Unable to load raw data <%s>\n", objName.c_str()));
        } else {
            SYNC_PRINT(("Loaded raw data\n"));
        }
    }


    printf("Will render <%s>\n", objName.c_str());



    SYNC_PRINT(("Starting render...\n"));
    cout << "Camera:" << cam << endl;
    cout << "Position:" << pose << endl;


    renderer.modelviewMatrix = cam.getKMatrix() * Matrix44(pose.inverted());
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    mesh->dumpInfo(cout);
    SYNC_PRINT(("Starting render...\n"));

    renderer.render(mesh, buffer);


    BMPLoader().save("meshdraw.bmp", buffer);
    buffer->drawDoubleBuffer(renderer.zBuffer, RGB24Buffer::STYLE_ZBUFFER);
    BMPLoader().save("meshdraw-z.bmp", buffer);
    renderer.dumpAllDebugs("meshdraw-");


    delete_safe(buffer);

    return 0;
}

#endif
