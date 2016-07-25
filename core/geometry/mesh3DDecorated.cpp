#include "mesh3DDecorated.h"


namespace corecvs
{

Mesh3DDecorated::Mesh3DDecorated() :
  hasTexCoords(false),
  hasNormals(false)
{
}

void Mesh3DDecorated::switchTextures(bool on)
{
    if (hasTexCoords == on)
        return;
    if (on) {
        texId.resize(faces.size(), Vector3d32(-1));
    } else {
        texId.clear();
    }
    hasTexCoords = on;
}

void Mesh3DDecorated::switchNormals(bool on)
{
    if (hasNormals == on)
        return;
    if (on) {
        normalId.resize(faces.size(), Vector3d32(-1));
    } else {
        normalId.clear();
    }
    hasNormals = on;
}

void Mesh3DDecorated::addAOB(const Vector3dd &c1, const Vector3dd &c2, bool addFaces)
{
    addAOB(c1, c2, addFaces);

  /*  textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));*/

}

void Mesh3DDecorated::transform(const Matrix44 &matrix)
{
    Mesh3D::transform(matrix);

    Matrix44 conjugateTransform = matrix.inverted().transposed();

    for (unsigned i = 0; i < normalCoords.size(); i++)
    {
        normalCoords[i] = conjugateTransform * normalCoords[i];
    }

}

void Mesh3DDecorated::clear()
{
    Mesh3D::clear();

    textureCoords.clear();
    normalCoords.clear();

    texId.clear();
    normalId.clear();
}

void Mesh3DDecorated::dumpInfo(ostream &out)
{
    Mesh3D::dumpInfo(out);
    out << " Normals On:"  << (hasNormals ? "on" : "off") << endl;
    out << " Textures On:"  << (hasTexCoords ? "on" : "off") << endl;

    out << "  Normals   :" << normalCoords.size() << endl;
    out << "  Textures  :" << textureCoords.size() << endl;
    out << "  Norm Idxes:" << normalId.size() << endl;
    out << "  Tex  Idxes:" << texId.size() << endl;
}

void Mesh3DDecorated::recomputeMeanNormals()
{
    switchNormals(true);

    normalCoords.clear();
    normalId.clear();

    /* We don't need this, just normalise */
    vector<int> counts;
    normalCoords.resize(vertexes.size(), Vector3dd::Zero());
    counts.resize(vertexes.size(), 0 );

    for (size_t f = 0; f < faces.size(); f++)
    {
        Triangle3dd triangle = getFaceAsTrinagle(f);
        Vector3dd normal = triangle.getNormal();
        for (int c = 0; c < 3; c++) {
            int vertexId = faces[f][c];
            normalCoords[vertexId] += normal;
            counts[vertexId] ++;
        }
        normalId.push_back(faces[f]);
    }

    for (size_t n = 0; n < normalCoords.size(); n++)
    {
        normalCoords[n] /= counts[n];
        normalCoords[n].normalise();
    }
}

bool Mesh3DDecorated::verify( void )
{
    if (faces.size() != texId.size() || faces.size() != normalCoords.size())
    {
        SYNC_PRINT(("Wrong face/texId/normalId index\n"));
        return false;
    }

    for (size_t i = 0; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (faces[i][j] > (int)vertexes.size() ) {
                SYNC_PRINT(("Wrong face index\n"));
                return false;
            }
        }
    }

    for (size_t i = 0; i < texId.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (texId[i][j] > (int)textureCoords.size() ) {
                SYNC_PRINT(("Wrong texture index\n"));
                return false;
            }
        }
    }

    for (size_t i = 0; i < normalId.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (normalId[i][j] > (int)normalCoords.size() && normalId[i][j] != -1) {
                SYNC_PRINT(("Wrong normal index for face %u - [%d %d %d]\n",
                     i, normalId[i][0], normalId[i][1], normalId[i][2]));
                return false;
            }
        }
    }
    return true;
}

} // namespace corecvs

