#include "core/fileformats/stlLoader.h"
#include "core/utils/utils.h"

namespace corecvs {

using std::istream;
using std::ostream;


static const int STL_BINARY_HEADER_SIZE = 80;

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

    SYNC_PRINT(("Magic String is <%s>\n", line.c_str()));
    if (!HelperUtils::startsWith(line, "solid"))
    {
        input.seekg (0, input.beg);
        return loadBinarySTL(input, mesh);
    }

    vector<Vector3dd> points;

    while (true)
    {
        if (!input) {
            break;
        }

        HelperUtils::getlineSafe(input, line);
        line = HelperUtils::removeLeading(line, " ");

        if (HelperUtils::startsWith(line, "facet"))
        {
            points.clear();
        }

        if (HelperUtils::startsWith(line, "outer"))
        {
            points.clear();
        }

        if (HelperUtils::startsWith(line, "vertex"))
        {
            std::istringstream work(line.substr(strlen("vertex")));
            Vector3dd vertex;
            work >> vertex.x() >> vertex.y() >> vertex.z();
            points.push_back(vertex);
        }

        if (HelperUtils::startsWith(line, "endfacet"))
        {
            if (points.size() == 3) {
                mesh.addTriangle(points[0], points[1], points[2]);
            }
        }

        if (HelperUtils::startsWith(line, "endsolid"))
        {
            break;
        }
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

void writeFloatVector(ostream &output, Vector3dd &value)
{
    float x = value.x();
    float y = value.y();
    float z = value.z();
    output.write((char *)&x, sizeof(float));
    output.write((char *)&y, sizeof(float));
    output.write((char *)&z, sizeof(float));
}

int STLLoader::loadBinarySTL(istream &input, Mesh3D &mesh)
{
    if (sizeof(float) != 4) {
        return 2;
    }

    char * buffer = new char [STL_BINARY_HEADER_SIZE];
    input.read(buffer, STL_BINARY_HEADER_SIZE);
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

int STLLoader::saveAsciiSTL(ostream &out, Mesh3D &mesh)
{
    out << "solid test" << endl;
    for (size_t f = 0; f < mesh.faces.size(); f++)
    {
        Triangle3dd face = mesh.getFaceAsTrinagle(f);
        Vector3dd normal = face.getNormal();

        out << "  facet normal " << normal.x() << " " << normal.y() << " " << normal.z() << endl;
        out << "     outer loop";

        out << "       vertex " << face.p1().x() << " " << face.p1().y() << " " << face.p1().z() << endl;
        out << "       vertex " << face.p2().x() << " " << face.p2().y() << " " << face.p2().z() << endl;
        out << "       vertex " << face.p3().x() << " " << face.p3().y() << " " << face.p3().z() << endl;

        out << "     end loop" << endl;
        out << "  endfacet" << endl;
    }
    out << "endsolid test" << endl;
    return 0;
}

int STLLoader::saveBinarySTL(ostream &out, Mesh3D &mesh)
{
    if (sizeof(float) != 4) {
        return 2;
    }

    char * buffer = new char [STL_BINARY_HEADER_SIZE];

    for (int i = 0; i < STL_BINARY_HEADER_SIZE; i++)
        buffer[i] = 0x00;

    out.write(buffer, STL_BINARY_HEADER_SIZE);
    if (out.bad()) {
        return 1;
    }

    uint32_t  numberTriangles = (int)mesh.faces.size();
    out.write((char *)&numberTriangles, sizeof(uint32_t));

    for (uint32_t i = 0; i < numberTriangles; i++)
    {

        Triangle3dd face = mesh.getFaceAsTrinagle(i);
        Vector3dd normal = face.getNormal();

        writeFloatVector(out, normal);

        writeFloatVector(out, face.p1());
        writeFloatVector(out, face.p2());
        writeFloatVector(out, face.p3());

        uint16_t attribute = 0;
        out.write((char *)&attribute, sizeof(attribute));

    }
    deletearr_safe(buffer);
    return 0;
}

} //namespace corecvs
