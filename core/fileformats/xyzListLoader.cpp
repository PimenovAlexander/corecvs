#include <sstream>
#include <iomanip>

#include "fileformats/xyzListLoader.h"
#include "utils/utils.h"

namespace corecvs {

using namespace std;

#define LOCAL_PRINT(X) if (trace) { SYNC_PRINT(X); }


int XYZListLoader::loadXYZ(istream &input, Mesh3D &mesh)
{
    SYNC_PRINT(("XYZListLoader::loadXYZ(): Loading:"));
    int points = 0;
    while (input)
    {
        string line;
        HelperUtils::getlineSafe (input, line);
        Vector3dd vertex = Vector3dd::Zero();
        istringstream work(line);
        work >> vertex.x() >> vertex.y() >> vertex.z();

        if (work) {
            mesh.addPoint(vertex);
        }

        if (mesh.hasAttributes) {
            int att;
            work >> att;

            if (work) {
                mesh.attributes.back() = att;
            }
        }
        points++;

        if (points % 100000 == 0)
             SYNC_PRINT(("#"));

    }
    SYNC_PRINT(("\n"));
    return 0;
}


int XYZListLoader::saveXYZ(ostream &out, Mesh3D &mesh)
{
    out.imbue(std::locale("C"));
    out << std::setprecision(15);

    for (size_t i = 0; i < mesh.vertexes.size(); i++)
    {
        Vector3dd &vertex = mesh.vertexes[i];
        out << vertex.x() << " " << vertex.y() << " " << vertex.z();
        if (mesh.hasAttributes) {
            out << " " << mesh.attributes[i];
        }
        if (mesh.hasColor) {
            out << " " << mesh.vertexesColor[i].r() << " " << mesh.vertexesColor[i].g() << " " << mesh.vertexesColor[i].b();
        }
        out << endl;
    }
    return 0;
}

int XYZListLoader::loadLabelList(istream &input, LabelList &list)
{
    int points = 0;
    while (input)
    {
        string line;
        HelperUtils::getlineSafe (input, line);
        Vector3dd vertex = Vector3dd::Zero();
        istringstream work(line);
        work >> vertex.x() >> vertex.y() >> vertex.z();

        std::string att;
        getline(work, att);
        if (work) {
            list.emplace_back(vertex, att);
        }
        points++;
    }
    return 0;
}

int XYZListLoader::saveLabelList(ostream &out, LabelList &list)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        Vector3dd &vertex = list[i].first;
        out << vertex.x() << " " << vertex.y() << " " << vertex.z() << " ";
        out << list[i].second;
        out << endl;
    }
    return 0;
}



XYZListLoader::XYZListLoader()
{

}

XYZListLoader::~XYZListLoader()
{
}

#undef LOCAL_PRINT

}
