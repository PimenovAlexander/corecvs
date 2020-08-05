#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"
#include "core/utils/utils.h"


#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace corecvs;
using namespace std;

static const double DEFAULT_SIZE = 0.01 * 2/3 * 1.5;

int main (int argc, char **argv)
{
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif


    CommandLineSetter s(argc, argv);

    if (s.hasOption("debug"))
    {
        Mesh3D mesh;
        mesh.switchColor();
        mesh.setColor(RGBColor::Gray());
        mesh.addPoint(Vector3dd::Zero());

        mesh.setColor(RGBColor::Red());
        mesh.addPoint(Vector3dd::OrtX());

        mesh.setColor(RGBColor::Green());
        mesh.addPoint(Vector3dd::OrtY());

        mesh.setColor(RGBColor::Blue());
        mesh.addPoint(Vector3dd::OrtZ());

        PLYLoader saver;
        ofstream file;
        file.open("debug.ply", ios::out);
        if (file.fail())
        {
            SYNC_PRINT(("Can't open mesh file for writing\n"));
            return 1;
        }
        saver.savePLY(file, mesh, PLYLoader::PlyFormat::BINARY_LITTLE_ENDIAN, true, true);
        return 0;
    }

    vector<string> nonPrefix = s.nonPrefix();
    if (nonPrefix.size() <= 1) {
        SYNC_PRINT(("Usage: \n"));
        SYNC_PRINT(("mesh3dplayground <in.ply> [--trace] [--decoration=<id>] [--dump] [--nocenter] [--double] [--limit=<d>]\n"));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("\t   --limit=<d> - only process d points\n"));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("\tDecoration:\n"));
        SYNC_PRINT(("\t 0 - cubes of [--size=<size>] %lf default\n", DEFAULT_SIZE));
        SYNC_PRINT(("\t 1 - do nothing just dry run \n"));
        SYNC_PRINT(("\n"));
        SYNC_PRINT(("Unsigned size: %d\n", (int)sizeof(unsigned int)));

        return 1;
    }

    string meshName = nonPrefix[1];

    bool trace = s.hasOption("trace");
    if (trace) {
        SYNC_PRINT(("Switching trace on.\n"));
    }

    Mesh3D mesh;    
    mesh.switchColor();
    bool result = false;

    MeshLoader loader;
    loader.trace = trace;
    result = loader.load(&mesh, meshName);
    if (!result) {
        SYNC_PRINT(("Unable to load <%s>\n", meshName.c_str()));
        return 2;
    }

    if (!mesh.hasColor) {
        SYNC_PRINT(("Mesh has no color\n"));
        return 3;
    }

    SYNC_PRINT(("Loaded <%s>\n", meshName.c_str()));
    mesh.dumpInfo();


    if (s.hasOption("dump"))
    {
        SYNC_PRINT(("Dumping input"));
        for (size_t i = 0; i < mesh.vertexes.size(); i++)
        {
            cout << mesh.vertexes[i] << " ";
            if (mesh.hasColor) {
                cout << mesh.vertexesColor[i] << " ";
            }
            cout << endl;
        }
        return 0;
    }

    if (!s.hasOption("nocenter"))
    {
        Vector3dd center = Vector3dd::Zero();
        for (size_t i = 0; i < mesh.vertexes.size(); i++)
        {
            center += mesh.vertexes[i];
        }
        if (mesh.vertexes.size() != 0)
        {
            center /= mesh.vertexes.size();
        }

        SYNC_PRINT(("Center is [%lf %lf %lf] shifting it to zero\n", center.x(), center.y(), center.z()));
        for (size_t i = 0; i < mesh.vertexes.size(); i++)
        {
            mesh.vertexes[i] -= center;
        }
    }

    Mesh3D meshExtended;
    meshExtended.switchColor();
    double tsize = s.getDouble("size", DEFAULT_SIZE);


    int decoration  = s.getInt("decoration", 0);

    const char *decNames[] = {"box", "none"};

    if (decoration < 0) decoration = 0;
    if (decoration >= CORE_COUNT_OF(decNames)) {
        decoration = CORE_COUNT_OF(decNames) - 1;
    }

    int limitPoints = s.getInt("limit", -1);
    if (limitPoints < 0) limitPoints = mesh.vertexes.size();


    SYNC_PRINT(("Adding <%s> for each of the %d vertexes of size %lf\n", decNames[decoration], (int)mesh.vertexes.size(), tsize));
    SYNC_PRINT(("We limit ourselfs to %d points\n", limitPoints));
    for (size_t i = 0; i < mesh.vertexes.size() && i < limitPoints; i++)
    {
        Vector3dd v = mesh.vertexes[i];
        RGBColor color = mesh.vertexesColor[i];


        //Vector3dd center = Vector3dd(1,1,1) / 3.0;
        meshExtended.setColor(color);
        /*meshExtended.addTriangle(
            v + (Vector3dd(1,0,0) - center)* tsize,
            v + (Vector3dd(0,1,0) - center)* tsize,
            v + (Vector3dd(0,0,1) - center)* tsize
        );*/
        switch (decoration) {
            case 0: meshExtended.addAOB(v - Vector3dd(1,1,1) * tsize,  v + Vector3dd(1,1,1) * tsize); break;
            default:
            case 1: meshExtended.addPoint(v); break;
        }
    }

    std::string baseName = HelperUtils::getFullPathWithoutExt(meshName);
    std::string outName = baseName + "-out.ply";

    bool storeDouble = s.hasOption("double");

    if (!storeDouble) {
        MeshLoader saver;
        saver.binary = true;
        saver.save(&meshExtended, outName);
    } else {
        PLYLoader saver;
        ofstream file;
        file.open(outName, ios::out);
        if (file.fail())
        {
            SYNC_PRINT(("Can't open mesh file for writing\n"));
            return 1;
        }
        saver.savePLY(file, meshExtended, PLYLoader::PlyFormat::BINARY_LITTLE_ENDIAN, true, true);
    }

    meshExtended.dumpInfo();
    return 0;
}

