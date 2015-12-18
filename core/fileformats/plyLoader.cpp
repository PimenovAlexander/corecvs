/**
 * \file plyLoader.cpp
 *
 * \date Nov 13, 2012
 **/

#include "plyLoader.h"



PLYLoader::PLYLoader()
{
}



/**
 *
 *  format ascii 1.0
    comment author: Greg Turk
    comment object: another cube
    element vertex 8
    property float x
    property float y
    property float z
    property uchar red                   { start of vertex color }
    property uchar green
    property uchar blue
    element face 7
    property list uchar int vertex_index  { number of vertices for each face }
    element edge 5                        { five edges in object }
    property int vertex1                  { index to first vertex of edge }
    property int vertex2                  { index to second vertex }
    property uchar red                    { start of edge color }
    property uchar green
    property uchar blue
 *
 *
 **/

int PLYLoader::loadPLY(istream &input, Mesh3D &mesh)
{
    char line[300];
    input.getline(line, sizeof(line) - 1);
    if (strcmp(line, "ply") != 0) {
        SYNC_PRINT(("Not a PLY file\n"));
        return 1;
    }

    const char *com_format     = "format";
    const char *com_comment    = "comment";
    const char *com_element    = "element";
    const char *com_property   = "property";
    const char *com_end_header = "end_header";
    const char *form_ascii     = "ascii";
    const char *form_binaryle  = "binary_little_endian";



    const char *el_vertex = "vertex";
    const char *el_face   = "face";

    int vertex_number = -1;
    int face_number = -1;

    enum PlyFormat {
        ASCII,
        BINARY_LITTLE_ENDIAN,
        OTHER
    };

    PlyFormat plyFormat = OTHER;

    while (!input.eof())
    {
        input.getline(line, sizeof(line) - 1);
        char *work = line;
        if (strncmp(work, com_format, strlen(com_format)) == 0)
        {
            work += strlen(com_format);
            while(*work == ' ' && *work != 0) work++;
            char *format = work;
            if (strncmp(format, form_ascii, strlen(form_ascii)) == 0)
            {
                plyFormat = ASCII;
                //SYNC_PRINT(("PLY ascii format\n"));
                continue;
            }

            if (strncmp(format, form_binaryle, strlen(form_binaryle)) == 0)
            {
                plyFormat = BINARY_LITTLE_ENDIAN;
                //SYNC_PRINT(("PLY binary format\n"));
                continue;
            }

            SYNC_PRINT(("PLY format not supported. Format here <%s>\n", format));
            return 1;
        }

        if (strncmp(work, com_comment, strlen(com_comment)) == 0)
        {
            work += strlen(com_comment);
            //printf("Skipping comment <%s>\n", work);
            continue;
        }

        if (strncmp(work, com_property, strlen(com_property)) == 0)
        {
            work += strlen(com_property);

            while(*work == ' ' && *work != 0) work++;

            if (strncmp(work, el_vertex, strlen(el_vertex)) == 0)
            {
               work += strlen(el_vertex);
               if (sscanf(work, "%d", &vertex_number) != 1) {
                   SYNC_PRINT(("Vertex number is not found"));
                   continue;
               }
               //SYNC_PRINT(("Vertex number %d", vertex_number));
               continue;
            }

            if (strncmp(work, el_face, strlen(el_face)) == 0)
            {
               work += strlen(el_face);
               if (sscanf(work, "%d", &face_number) != 1) {
                   SYNC_PRINT(("Face number is not found"));
                   continue;
               }
               //SYNC_PRINT(("Face number %d", face_number));
               continue;
            }
            //SYNC_PRINT(("Found unsupported property <%s>\n", work));
            continue;
        }

        if (strncmp(work, com_element, strlen(com_element)) == 0)
        {
            work += strlen(com_element);

            while(*work == ' ' && *work != 0) work++;

            if (strncmp(work, el_vertex, strlen(el_vertex)) == 0)
            {
               work += strlen(el_vertex);
               if (sscanf(work, "%d", &vertex_number) != 1) {
                   SYNC_PRINT(("Vertex number is not found"));
                   continue;
               }
               //SYNC_PRINT(("Vertex number %d\n", vertex_number));
               continue;
            }

            if (strncmp(work, el_face, strlen(el_face)) == 0)
            {
               work += strlen(el_face);
               if (sscanf(work, "%d", &face_number) != 1) {
                   SYNC_PRINT(("Face number is not found"));
                   continue;
               }
               //SYNC_PRINT(("Face number %d\n", face_number));
               continue;
            }
            printf("Found unsupported element <%s>\n", work);
            continue;
        }

        if (strncmp(work, com_end_header, strlen(com_end_header)) == 0)
        {
            break;
        }
    }

    if (vertex_number == -1 || face_number == -1)
    {
        SYNC_PRINT(("Necessary data was missing form the header\n"));
        return 1;
    }

    if (plyFormat == ASCII)
    {
        for (int i = 0; i < vertex_number; i++)
        {
            input.getline(line, sizeof(line) - 1);
            Vector3dd vertex;
            if (sscanf(line, "%lf %lf %lf", &(vertex.x()), &(vertex.y()), &(vertex.z())) != 3) {
                SYNC_PRINT(("Corrupted vertex %d <%s>\n", i, line));
                return 1;
            }
            mesh.vertexes.push_back(vertex);
        }

        for (int i = 0; i < face_number; i++)
        {
            input.getline(line, sizeof(line) - 1);
            char *work = line;
            while(*work == ' ' && *work != 0) work++;
            int points;
            if (sscanf(line, "%d", &points) != 1) {
                SYNC_PRINT(("Corrupted face number %d\n", i));
                return 1;
            }
            if (points != 3) {
                SYNC_PRINT(("We only support faces with 3 sides not %d\n", points));
                continue;
            }

            while(*work != ' ' && *work != 0) work++;
            Vector3d32 face;

            if (sscanf(work, "%d %d %d", &(face.x()), &(face.y()), &(face.z())) != 3) {
                SYNC_PRINT(("Corrupted face number %d\n", i));
                return 1;
            }

            if (!face.isInCube(
                    Vector3d32(                0,                 0,                0),
                    Vector3d32(vertex_number - 1, vertex_number - 1, vertex_number - 1)))
            {
                SYNC_PRINT(("Vertex index is wrong on face %i\n", i));
                return 1;
            }
            //printf("%d %d %d\n", face.x(), face.y(), face.z());

            mesh.faces.push_back(face);
        }
    } else {
        for (int i = 0; i < vertex_number; i++)
        {
            float f;
            Vector3dd vertex;
            input.read((char *)&f, sizeof(f)); vertex.x() = f;
            input.read((char *)&f, sizeof(f)); vertex.y() = f;
            input.read((char *)&f, sizeof(f)); vertex.z() = f;
            mesh.vertexes.push_back(vertex);
        }
    }

    return 0;
}


PLYLoader::~PLYLoader()
{
}

