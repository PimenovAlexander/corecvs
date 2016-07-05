#include <fstream>

#include "meshLoader.h"
#include "plyLoader.h"
#include "stlLoader.h"

namespace corecvs {
using namespace std;

bool MeshLoader::endsWith(const string &fileName, const char *extention)
{
    size_t extLen = strlen(extention);
    if (fileName.compare(fileName.length() - extLen, extLen, extention) == 0)
        return true;
    return false;
}

MeshLoader::MeshLoader() :
    trace(false)
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

    if (endsWith(fileName, PLY_RES))
    {
        SYNC_PRINT(("MeshLoader::load(): Loading PLY <%s>\n", fileName.c_str()));
        PLYLoader loader;
        loader.trace = trace;
        if (loader.loadPLY(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }

    if (endsWith(fileName, STL_RES))
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

    mesh->dumpInfo(cout);
    return true;
}

bool MeshLoader::save(Mesh3D *mesh, const string &fileName)
{
    ofstream file;
    file.open(fileName, ios::out);
    if (file.fail())
    {
        SYNC_PRINT(("MeshLoader::save(): Can't open mesh file <%s> for writing/n", fileName.c_str()));
        return false;
    }

    if (endsWith(fileName, PLY_RES))
    {
        SYNC_PRINT(("MeshLoader::save(): Saving PLY <%s>\n", fileName.c_str()));
        int res = mesh->dumpPLY(file);
        if (res != 0)
        {
           SYNC_PRINT(("MeshLoader::save(): Unable to save mesh code=%d\n", res ));
           file.close();
           return false;
        }
    }

    if (endsWith(fileName, STL_RES))
    {
        SYNC_PRINT(("MeshLoader::save(): Saving binary STL <%s>\n", fileName.c_str()));
        STLLoader loader;
        if (loader.saveBinarySTL(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::save(): Unable to load mesh"));
           file.close();
           return false;
        }
    }


    return true;
}

std::string MeshLoader::extentionList()
{
    return string("*") + string(PLY_RES) + " *" + string(STL_RES);
}

} //namespace corecvs
