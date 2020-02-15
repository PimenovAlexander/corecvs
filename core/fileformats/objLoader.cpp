#include <sstream>
#include <regex>
#include <fstream>

#include "core/fileformats/objLoader.h"
#include "core/utils/utils.h"
#include "core/buffers/bufferFactory.h"

#include "core/fileformats/bmpLoader.h"

using namespace std;

namespace corecvs {


static const char *com_vertex          = "v";
static const char *com_vertex_texture  = "vt";
static const char *com_vertex_normal   = "vn";
static const char *com_face            = "f";
static const char *com_smoothing       = "s";
static const char *com_grouping        = "g";


static const char *com_use_material    = "usemtl";
static const char *com_material_lib    = "mtllib";

OBJLoader::OBJLoader()
{
}

OBJLoader::~OBJLoader()
{
}

#define LOCAL_PRINT(X) if (trace) { SYNC_PRINT(X); }

int OBJLoader::loadOBJ(istream &input, Mesh3DDecorated &mesh)
{
    int texName = 0;

    string line;
    while (!input.eof() && !input.bad())
    {
        HelperUtils::getlineSafe (input, line);
        while (!line.empty() && line.back() == '\\') {
               string line2;
               HelperUtils::getlineSafe (input, line2);
               line.pop_back();
               line += line2;
        }

        if (HelperUtils::startsWith(line, "#")) {
            if (trace) cout << "Skipping comment " << line << endl;
            continue;
        }

        istringstream work(line);
        string command;
        work >> command;

        //LOCAL_PRINT(("Input line: <%s>\n", line.c_str()));

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

            //LOCAL_PRINT(("Face line: %s\n", work.str().c_str()));
            vector<string> strs;
            while (!work.eof()) {
                string part;
                work >> part;
                if (!part.empty()) {
                    strs.push_back(part);
                }
            }

            //LOCAL_PRINT(("Face parts: %d\n", strs.size()));

            vector<int32_t> face  (strs.size(), -1);
            vector<int32_t> normId(strs.size(), -1);
            vector<int32_t> texId (strs.size(), -1);

            for (size_t i = 0; i < strs.size(); i++)
            {

                //LOCAL_PRINT(("Attribute: %s\n", strs[i].c_str()));
                std::stringstream splitter(strs[i]);
                std::string part;

                /* We now parse group i.e */
                /* 2684//856              */

                for (int j = 0; j < 3 && std::getline(splitter, part, '/'); j++)
                {
                    if (j == 0 && !part.empty()) {
                        int id = std::stoi(part);
                        face[i] = (id - 1);
                    }

                    if (j == 1 && !part.empty()) {
                        int id = std::stoi(part);
                        texId[i] = (id - 1);
                    }

                    if (j == 2 && !part.empty()) {
                        int id = std::stoi(part);
                        normId[i] = (id - 1);
                    }
                }
            }

            for (size_t i = 2; i < face.size(); i++) {
                Vector3d32 tface(face[0], face[i-1], face[i]);
                mesh.addFace(tface);
                Vector4d32 ttexId(texId[0], texId[i-1], texId[i], texName);
                mesh.texId.push_back(ttexId);
                Vector3d32 tnormId(normId[0], normId[i-1], normId[i]);
                mesh.normalId.push_back(tnormId);
            }
        }
        if (command == com_use_material)
        {
            string name;
            work >> name;

            texName = 0;
            for (size_t t = 0; t < mesh.materials.size(); t++)
            {
                if (mesh.materials[t].name == name)
                {
                    texName = (int)t;
                }
            }

             if(trace) cout << "Use material command: <" << name << "> id " << texName << " (" << mesh.materials.size() << ")" << endl;
        }
        if (command == com_material_lib)
        {
            if(trace) cout << "Material library command: This is not fully supported" << endl;
        }
        if (command == com_smoothing)
        {
            static int smoothing_count = 0;
            smoothing_count++;
            if (smoothing_count < 10) {
                if(trace) cout << "smoothing command: This is not fully supported" << endl;
            }
            if (smoothing_count == 10) {
                if(trace) cout << "smoothing command: This is not fully supported - futher would be suppressed" << endl;
            }
        }
        if (command == com_grouping)
        {
            if(trace) cout << "grouping command: This is not fully supported" << endl;
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

int OBJLoader::loadMaterials(istream &input, vector<OBJMaterial> &materials, const std::string &path)
{
    int count = 0;
    string line;

    OBJMaterial material;

    while (input.good())
    {
        count++;
        HelperUtils::getlineSafe (input, line);

        if (trace) cout << "Line " << count << " <" <<  line << ">" << endl;

        if (HelperUtils::startsWith(line, "#")) {
            if (trace) cout << "OBJLoader::loadMaterial: Skipping comment " << line << endl;
            continue;
        }

        istringstream work(line);
        string command;
        work >> command;

        if (trace) cout << "OBJLoader::loadMaterial: command: " << command << endl;

        if (command == "newmtl") {
            if (!material.name.empty())
                materials.push_back(material);
            material = OBJMaterial();

            std::string material_name;
            work >> material_name;
            material.name = material_name;
        }



        int koef_id = OBJMaterial::KOEF_LAST;
        if (command == "Ka")
            koef_id = OBJMaterial::KOEF_AMBIENT;
        if (command == "Kd")
            koef_id = OBJMaterial::KOEF_DIFFUSE;
        if (command == "Ks")
            koef_id = OBJMaterial::KOEF_SPECULAR;
        if (command == "Tf")
            koef_id = OBJMaterial::KOEF_TRANSMISSION_FILTER;


        if (koef_id != OBJMaterial::KOEF_LAST)
        {
            if (trace) cout << "Will load koef" << endl;
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
            if (trace) cout << "Will load texture <" << tex_name << "> ";
            if (trace) cout << "full path <" << fullpath << ">" << endl;

            RGB24Buffer *texture = BufferFactory::getInstance()->loadRGB24Bitmap(fullpath);
            if (texture != NULL) {
                if (trace) cout << "Texture <" << texture->getSize() << ">" << endl;
            } else {
                cout << "Failed to load texture" << endl;
            }

            material.tex[tex_id] = texture;
        }
    }
    if (!material.name.empty())
        materials.push_back(material);

    cout << "Loaded :" << materials.size() << endl;
    for (OBJMaterial &m : materials)
    {
        cout << "    <" << m.name << ">" << endl;
    }


   return 0;
}

int OBJLoader::loadOBJSimple(istream &input, Mesh3D &mesh)
{
    string line;
    while (!input.eof())
    {
        HelperUtils::getlineSafe (input, line);

        if (HelperUtils::startsWith(line, "#")) {
            if (trace) cout << "Skipping comment " << line << endl;
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
            //LOCAL_PRINT(("Face line: %s\n", work.str().c_str()));
            vector<string> strs;
            while (!work.eof()) {
                string part;
                work >> part;
                strs.push_back(part);
            }

            //LOCAL_PRINT(("Face parts: %d\n", strs.size()));

            vector<int32_t> face;

            for (size_t i = 0; i < strs.size(); i++)
            {
                // LOCAL_PRINT(("Attribute: %s\n", strs[i].c_str()));
                std::stringstream splitter(strs[i]);
                std::string part;

                for (int j = 0; j < 3 && std::getline(splitter, part, '/'); j++)
                {
                    if (j == 0) {
                        int id = std::stoi(part);
                        face.push_back(id - 1);
                    }
                }
            }

            for (size_t i = 2; i < face.size(); i++) {
                Vector3d32 tface(face[0], face[i-1], face[i]);
                mesh.addFace(tface);
                //cout << "+";
            }
            //cout << endl;
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



int OBJLoader::saveMaterials(const string &fileName, vector<OBJMaterial> &materials, const string &/*path*/)
{
    ofstream out;
    out.open(fileName, ios::out);
    if (out.fail())
    {
        SYNC_PRINT(("MeshLoader::saveMaterials(): Can't open mesh file <%s> for writing\n", fileName.c_str()));
        return false;
    }

    for (size_t t = 0; t < materials.size(); t++ )
    {
        out << "newmtl " << materials[t].name << endl;

        std::stringstream texname;
        texname << "tex" << t << ".bmp";

        out << "map_Ka " << texname.str() << endl;

        if (materials[t].tex[OBJMaterial::TEX_AMBIENT] != NULL)
        {
            BMPLoader().save(texname.str(), materials[t].tex[OBJMaterial::TEX_AMBIENT]);
        }
    }

    out.close();
    return 0;
}

int OBJLoader::saveObj(const  string &fileName, Mesh3DDecorated &mesh)
{
    vector<Vector3dd>  &vertexes = mesh.vertexes;
    vector<Vector3d32> &faces    = mesh.faces;

    vector<Vector2dd> &texuv    = mesh.textureCoords;
    vector<Vector3dd> &normals  = mesh.normalCoords;
    //vector<Vector2d32> &edges    = mesh.edges;

    int currentMaterial = -1;

    ofstream out;
    out.open(fileName + ".obj", ios::out);
    if (out.fail())
    {
        SYNC_PRINT(("MeshLoader::save(): Can't open mesh file <%s> for writing\n", fileName.c_str()));
        return false;
    }

    out << "mtllib " << fileName << ".mtl" << endl;


    for (unsigned i = 0; i < vertexes.size(); i++)
    {
        out << "v "
            << vertexes[i].x() << " "
            << vertexes[i].y() << " "
            << vertexes[i].z() << " " << endl;

    }

    if (mesh.hasTexCoords)
    {
        for (unsigned i = 0; i < texuv.size(); i++)
        {
            out << "vt "
                << texuv[i].x() << " "
                << texuv[i].y() << " " << endl;

        }
    }

    if (mesh.hasNormals)
    {
        for (unsigned i = 0; i < normals.size(); i++)
        {
            out << "vn "
                << normals[i].x() << " "
                << normals[i].y() << " "
                << normals[i].z() << " " << endl;
        }
    }

    for (unsigned i = 0; i < faces.size(); i++)
    {
        if (mesh.hasTexCoords) {
            Vector4d32 &texid = mesh.texId[i];
            int t = texid[3];
            if (t != currentMaterial && t < (int)mesh.materials.size()) {
                out << "usemtl " << mesh.materials[t].name << endl;
                currentMaterial = t;
            }
        }

        out << "f ";
        out << (faces[i].x() + 1);
        out << "/";
        if (mesh.hasTexCoords) { out << mesh.texId[i][0] + 1; }
        out << "/";
        if (mesh.hasNormals  ) { out << mesh.normalId[i][0] + 1; }
        out << " ";

        out << (faces[i].y() + 1);
        out << "/";
        if (mesh.hasTexCoords) { out << mesh.texId[i][1] + 1; }
        out << "/";
        if (mesh.hasNormals  ) { out << mesh.normalId[i][1] + 1; }
        out << " ";

        out << (faces[i].z() + 1);
        out << "/";
        if (mesh.hasTexCoords) { out << mesh.texId[i][2] + 1; }
        out << "/";
        if (mesh.hasNormals  ) { out << mesh.normalId[i][2] + 1; }
        out << " ";
        out << std::endl;
    }

    return 0;

}

}
