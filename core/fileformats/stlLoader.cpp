#include "stlLoader.h"
#include "utils.h"

STLLoader::STLLoader()
{
}

STLLoader::~STLLoader()
{

}

int STLLoader::loadAsciiSTL(istream &input, Mesh3D &mesh)
{
    string line;
    HelperUtils::getlineSafe(input, line);

    SYNC_PRINT(("Magic String is %s", line.c_str()));
    if (!HelperUtils::startsWith(line, "solid"))
    {
        input.seekg (0, input.beg);
        loadBinarySTL(input, mesh);
        return 1;
    }

    HelperUtils::getlineSafe(input, line);
    while (!HelperUtils::startsWith(line, "endsolid"))
    {



    }
    return 0;
}

Vector3dd readFloatVector(istream &input)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    input.read((char *)&x, sizeof(float));
    input.read((char *)&y, sizeof(float));
    input.read((char *)&z, sizeof(float));
    return Vector3dd(x,y,z);
}

int STLLoader::loadBinarySTL(istream &input, Mesh3D &mesh)
{
    if (sizeof(float) != 4) {
        return 2;
    }

    static const int HEADER_SIZE = 80;

    char * buffer = new char [HEADER_SIZE];
    input.read(buffer, HEADER_SIZE);
    if (input.bad()) {
        return 1;
    }

    uint32_t  numberTriangles = 0;
    input.read((char *)&numberTriangles, sizeof(uint32_t));

    for (uint32_t i = 0; i < numberTriangles; i++)
    {
        /*Ok normal is skipped*/
        /*Vector3dd normal =*/ readFloatVector(input);

        Vector3dd v1 = readFloatVector(input);
        Vector3dd v2 = readFloatVector(input);
        Vector3dd v3 = readFloatVector(input);

        uint16_t attribute;
        input.read((char *)&attribute, sizeof(attribute));

        mesh.addTriangle(v1,v2,v3);
    }
    deletearr_safe(buffer);
    return 0;
}
