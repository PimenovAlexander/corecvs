#include <fstream>

#include "meshLoader.h"
#include "plyLoader.h"
#include "stlLoader.h"

namespace corecvs {
using namespace std;

MeshLoader::MeshLoader()
{

}

static const char *PLY_RES = ".ply";
static const char *STL_RES = ".stl";

bool MeshLoader::load(Mesh3D *mesh, const string &fileName)
{
    ifstream file;
    file.open(fileName, ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("MeshLoader::load(): Can't open mesh file <%s>/n", fileName.c_str()));
        return false;
    }

    const char *PLY_RES = ".ply";
    if (fileName.compare(fileName.length() - strlen(PLY_RES), strlen(PLY_RES), PLY_RES) == 0)
    {
        SYNC_PRINT(("MeshLoader::load(): Loading PLY <%s>\n", fileName.c_str()));
        PLYLoader loader;
        if (loader.loadPLY(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }


    if (fileName.compare(fileName.length() - strlen(STL_RES), strlen(STL_RES), STL_RES) == 0)
    {
        SYNC_PRINT(("MeshLoader::load(): Loading STL <%s>\n", fileName.c_str()));
        STLLoader loader;
        if (loader.loadBinarySTL(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh"));
           file.close();
           return false;
        }
    }

    cout << "Loaded mesh:" << endl;
    cout << " Edges   :" << mesh->edges.size() << endl;
    cout << " Vertexes:" << mesh->vertexes.size() << endl;
    cout << " Faces   :" << mesh->faces.size() << endl;
    cout << " Bounding box " << mesh->getBoundingBox() << endl;

    return true;
}

std::string MeshLoader::extentionList()
{
    return string("*") + string(PLY_RES) + " *" + string(STL_RES);
}

} //namespace corecvs
