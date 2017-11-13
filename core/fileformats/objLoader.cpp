#include <sstream>
#include <regex>

#include "core/fileformats/objLoader.h"
#include "core/utils/utils.h"
#include "core/buffers/bufferFactory.h"

using namespace std;

namespace corecvs {


static const char *com_vertex          = "v";
static const char *com_vertex_texture  = "vt";
static const char *com_vertex_normal   = "vn";
static const char *com_face            = "f";

OBJLoader::OBJLoader()
{
}

OBJLoader::~OBJLoader()
{
}

#define LOCAL_PRINT(X) if (trace) { SYNC_PRINT(X); }

int OBJLoader::loadOBJ(istream &input, Mesh3DDecorated &mesh)
{
    string line;
    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (HelperUtils::startsWith(line, "#")) {
            cout << "Skipping comment " << line << endl;
            continue;
        }

        istringstream work(line);
        string command;
        work >> command;

        // LOCAL_PRINT(("Input line: %s\n", line.c_str()));

        if (command == com_vertex)
        {
            Vector3dd vertex;
            work >> vertex.x() >> vertex.y() >> vertex.z();

            mesh.addPoint(vertex);
        }
        if (command == com_vertex_texture)
        {
            Vector2dd tex;
            work >> tex.x() >> tex.y();
         //   LOCAL_PRINT(("Tex: %lf %lf\n", tex.x(), tex.y()));
            mesh.textureCoords.push_back(tex);
        }
        if (command == com_vertex_normal)
        {
            Vector3dd normal;
            work >> normal.x() >> normal.y() >> normal.z();
         //   LOCAL_PRINT(("Normal line: %lf %lf %lf\n", normal.x(), normal.y(), normal.z()));
            mesh.normalCoords.push_back(normal);
        }
        if (command == com_face)
        {
            string strs[3];
            work >> strs[0] >> strs[1] >> strs[2];
            Vector3d32 face;
            Vector3d32 normId(-1);
            Vector3d32 texId(-1);

            //LOCAL_PRINT(("Face line: %s\n", work.str().c_str()));

            for (int i = 0; i < 3; i++)
            {
                // LOCAL_PRINT(("Attribute: %s\n", strs[i].c_str()));
                std::stringstream splitter(strs[i]);
                std::string part;

                for (int j = 0; j < 3 && std::getline(splitter, part, '/'); j++)
                {
                    if (j == 0) {
                        int id = std::stoi(part);
                        face[i] = id - 1;
                    }

                    if (j == 1) {
                        int id = std::stoi(part);
                        texId[i] = id - 1;
                    }

                    if (j == 2) {
                        int id = std::stoi(part);
                        normId[i] = id - 1;
                    }
                }
            }
            mesh.addFace(face);
            mesh.texId.push_back(texId);
            mesh.normalId.push_back(normId);
        }
    }

    if (!mesh.normalCoords.empty())
        mesh.hasNormals = true;

    if (!mesh.textureCoords.empty())
        mesh.hasTexCoords = true;

    /* sanity checking */
    mesh.dumpInfo(cout);

    mesh.verify();

    return 0;
}

int OBJLoader::loadMaterial(istream &input, OBJMaterial &material, const std::string &path)
{
    string line;
    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (HelperUtils::startsWith(line, "#")) {
            cout << "Skipping comment " << line << endl;
            continue;
        }

        istringstream work(line);
        string command;
        work >> command;

        cout << "command: " << command << endl;

        int koef_id = OBJMaterial::KOEF_LAST;
        if (command == "Ka")
            koef_id = OBJMaterial::KOEF_AMBIENT;
        if (command == "Kd")
            koef_id = OBJMaterial::KOEF_DIFFUSE;
        if (command == "Ks")
            koef_id = OBJMaterial::KOEF_SPECULAR;

        if (koef_id != OBJMaterial::KOEF_LAST)
        {
            cout << "Will load koef" << endl;
            work >> material.koefs[koef_id].x();
            work >> material.koefs[koef_id].y();
            work >> material.koefs[koef_id].z();
        }


        int tex_id = OBJMaterial::TEX_LAST;
        std::string tex_name;
        if (command == "map_Ka")
            tex_id =  OBJMaterial::TEX_AMBIENT;
        if (command == "map_Kd")
            tex_id =  OBJMaterial::TEX_DIFFUSE;
        if (command == "map_Ks")
            tex_id =  OBJMaterial::TEX_SPECULAR;
        if (command == "map_bump")
            tex_id =  OBJMaterial::TEX_BUMP;

        if (tex_id != OBJMaterial::TEX_LAST)
        {
            work >> tex_name;
            std::string fullpath = path + PATH_SEPARATOR + tex_name;
            cout << "Will load texture <" << tex_name << ">";
            cout << "full path <" << fullpath << ">" << endl;

            RGB24Buffer *texture = BufferFactory::getInstance()->loadRGB24Bitmap(fullpath);
            if (texture != NULL) {
                cout << "Texture <" << texture->getSize() << ">" << endl;
            } else {
                cout << "Failed to load texture" << endl;
            }

            material.tex[tex_id] = texture;
        }
    }

    cout << "Loaded" << endl;
    cout << material << endl;
	return 0;
}

int OBJLoader::loadOBJSimple(istream &input, Mesh3D &mesh)
{
    string line;
    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (HelperUtils::startsWith(line, "#")) {
            cout << "Skipping comment " << line << endl;
            continue;
        }

        istringstream work(line);
        string command;
        work >> command;

        // LOCAL_PRINT(("Input line: %s\n", line.c_str()));

        if (command == com_vertex)
        {
            Vector3dd vertex;
            work >> vertex.x() >> vertex.y() >> vertex.z();

            mesh.addPoint(vertex);
        }
        if (command == com_vertex_texture)
        {
        }
        if (command == com_vertex_normal)
        {
        }
        if (command == com_face)
        {
            string strs[3];
            work >> strs[0] >> strs[1] >> strs[2];
            Vector3d32 face;

            for (int i = 0; i < 3; i++)
            {
                // LOCAL_PRINT(("Attribute: %s\n", strs[i].c_str()));
                std::stringstream splitter(strs[i]);
                std::string part;

                for (int j = 0; j < 1 && std::getline(splitter, part, '/'); j++)
                {
                    int id = std::stoi(part);
                    face[i] = id - 1;
                }
            }
            mesh.addFace(face);
        }


    }

    return 0;
}

int OBJLoader::saveOBJSimple(ostream &out, Mesh3D &mesh)
{
    vector<Vector3dd>  &vertexes = mesh.vertexes;
    vector<Vector3d32> &faces    = mesh.faces;
    //vector<Vector2d32> &edges    = mesh.edges;

    for (unsigned i = 0; i < vertexes.size(); i++)
    {
        out << "v "
            << vertexes[i].x() << " "
            << vertexes[i].y() << " "
            << vertexes[i].z() << " " << endl;

    }

    for (unsigned i = 0; i < faces.size(); i++)
    {
        out << "f "
            << (faces[i].x() + 1) << " "
            << (faces[i].y() + 1) << " "
            << (faces[i].z() + 1) << " ";
        out << std::endl;
    }

    return 0;
}

}
