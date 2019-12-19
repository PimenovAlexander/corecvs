#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace corecvs;

int main (int argc, char **argv)
{
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    CommandLineSetter s(argc, argv);

    vector<string> nonPrefix = s.nonPrefix();
    if (nonPrefix.size() <= 1) {
        SYNC_PRINT(("Usage: \n"));
        SYNC_PRINT(("mesh3dplayground <in.ply>\n"));
        return 1;
    }
    string meshName = nonPrefix[1];

    Mesh3D mesh;
    mesh.switchColor();
    bool result = MeshLoader().load(&mesh, meshName);
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

    Mesh3D meshExtended;
    meshExtended.switchColor();
    double tsize = 0.01;

    SYNC_PRINT(("Adding triangle for each of the %d vertexes\n", (int)mesh.vertexes.size()));
    for (int i = 0; i < mesh.vertexes.size(); i++)
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
        meshExtended.addAOB(v - Vector3dd(1,1,1) * tsize,  v + Vector3dd(1,1,1) * tsize);
    }

    MeshLoader saver;
    saver.binary = true;
    saver.save(&meshExtended, "out.ply");
    return 0;
}

