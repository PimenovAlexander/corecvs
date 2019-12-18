#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"
#include "core/fileformats/stlLoader.h"
#include "core/fileformats/objLoader.h"
#include "core/fileformats/gcodeLoader.h"
#include "core/fileformats/xyzListLoader.h"
#include "core/utils/utils.h"
#include <fstream>

using namespace std;

namespace corecvs {

MeshLoader::MeshLoader() : trace(false)
{}

static const char *PLY_EXT = ".ply";
static const char *STL_EXT = ".stl";
static const char *OBJ_EXT = ".obj";
static const char *GCODE_EXT = ".gcode";
static const char *XYZ_EXT = ".xyz";

bool MeshLoader::load(Mesh3D *mesh, const string &fileName)
{
    bool maybeBinary = HelperUtils::endsWith(fileName, PLY_EXT);    // PLY format may have a binary stream

    ifstream file;
    file.open(fileName, maybeBinary ? (ios::in | ios::binary) : ios::in);
    if (file.fail())
    {
        SYNC_PRINT(("MeshLoader::load(): Can't open mesh file <%s>\n", fileName.c_str()));
        return false;
    }

    if (HelperUtils::endsWith(fileName, PLY_EXT))
    {
        if (trace) SYNC_PRINT(("MeshLoader::load(): Loading PLY <%s>\n", fileName.c_str()));
        PLYLoader loader;
        loader.trace = trace;
        if (loader.loadPLY(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }

    if (HelperUtils::endsWith(fileName, STL_EXT))
    {
        if (trace) SYNC_PRINT(("MeshLoader::load(): Loading STL <%s>\n", fileName.c_str()));
        STLLoader loader;
#if 0
        if (loader.loadBinarySTL(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh"));
           file.close();
           return false;
        }
#else
        if (loader.loadAsciiSTL(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
#endif
    }

    if (HelperUtils::endsWith(fileName, OBJ_EXT))
    {
        if (trace) SYNC_PRINT(("MeshLoader::load(): Loading OBJ <%s>\n", fileName.c_str()));
        OBJLoader loader;
        if (loader.loadOBJSimple(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }

    if (HelperUtils::endsWith(fileName, XYZ_EXT))
    {
        if (trace) SYNC_PRINT(("MeshLoader::load(): Loading XYZ <%s>\n", fileName.c_str()));
        XYZListLoader loader;
        if (loader.loadXYZ(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }

    if (HelperUtils::endsWith(fileName, GCODE_EXT))
    {
        if (trace) SYNC_PRINT(("MeshLoader::load(): Loading GCODE <%s>\n", fileName.c_str()));
        GcodeLoader loader;
        if (loader.loadGcode(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::load(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }

    //mesh->dumpInfo(cout);
    return true;
}

bool MeshLoader::save(Mesh3D *mesh, const string &fileName)
{
    ofstream file;
    file.open(fileName, ios::out);
    if (file.fail())
    {
        SYNC_PRINT(("MeshLoader::save(): Can't open mesh file <%s> for writing\n", fileName.c_str()));
        return false;
    }

    if (HelperUtils::endsWith(fileName, PLY_EXT))
    {
        SYNC_PRINT(("MeshLoader::save(): Saving PLY <%s>\n", fileName.c_str()));
        PLYLoader loader;
        int res = loader.savePLY(file, *mesh, binary ? PLYLoader::BINARY_LITTLE_ENDIAN : PLYLoader::ASCII);
        if (res != 0)
        {
           SYNC_PRINT(("MeshLoader::save(): Unable to save mesh code=%d\n", res ));
           file.close();
           return false;
        }
    }

    if (HelperUtils::endsWith(fileName, OBJ_EXT))
    {
        SYNC_PRINT(("MeshLoader::save(): Saving OBJ <%s>\n", fileName.c_str()));
        OBJLoader loader;
        int res = loader.saveOBJSimple(file, *mesh);
        if (res != 0)
        {
           SYNC_PRINT(("MeshLoader::save(): Unable to save mesh code=%d\n", res ));
           file.close();
           return false;
        }
    }

    if (HelperUtils::endsWith(fileName, STL_EXT))
    {
        SYNC_PRINT(("MeshLoader::save(): Saving binary STL <%s>\n", fileName.c_str()));
        STLLoader loader;
        if (loader.saveBinarySTL(file, *mesh) != 0)
        {
           SYNC_PRINT(("MeshLoader::save(): Unable to load mesh\n"));
           file.close();
           return false;
        }
    }


    return true;
}

std::string MeshLoader::extentionList()
{
    return string("*") + string(PLY_EXT)
               + " *" + string(STL_EXT)
               + " *" + string(OBJ_EXT)
               + " *" + string(GCODE_EXT)
               + " *" + string(XYZ_EXT);
}

} //namespace corecvs
