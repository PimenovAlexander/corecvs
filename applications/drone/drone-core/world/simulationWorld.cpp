#include <fstream>

#include <core/fileformats/objLoader.h>
#include <core/utils/utils.h>

#include "simulationWorld.h"
#include "jsonModernReader.h"

using namespace corecvs;

SimulationWorld::SimulationWorld()
{

}

void SimulationWorld::load(const std::string &filename)
{
    JSONModernReader reader(filename);

    if (reader.hasError())
    {
        SYNC_PRINT(("SimulationWorld::load(%s): failed\n", filename.c_str()));
        return;
    }
    accept<JSONModernReader>(reader);

    for (size_t i = 0; i < meshes.size(); i++)
    {
        std::ifstream file;
        OBJLoader loader;

        loader.trace = false;

        std::string name = meshes[i].meshName;
        std::string basename = name.substr(0, name.length() - 4);
        std::string path = HelperUtils::getPathWithoutFilename(name);
        std::string mtlFile = basename + ".mtl";

        cout << "Loading mesh name:" << name << endl;
        cout << "Loading mesh material name:" << mtlFile << endl;
        cout << "Loading mesh path:" << path << endl;

        std::ifstream materialFile;
        materialFile.open(mtlFile, std::ios::in);
        loader.loadMaterials(materialFile, meshes[i].mesh.materials, path);
        materialFile.close();

        file.open(meshes[i].meshName, std::ios::in);
        loader.loadOBJ(file, meshes[i].mesh);
        file.close();




    }
}
