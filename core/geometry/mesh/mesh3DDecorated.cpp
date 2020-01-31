#include "core/geometry/mesh/mesh3DDecorated.h"


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
        texId.resize(faces.size(), Vector4d32(-1,-1,-1, currentTexture));
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

void Mesh3DDecorated::addFace(const Vector3d32 &faceId)
{
    Mesh3D::addFace(faceId);
    if (hasTexCoords) {
        texId.push_back(Vector4d32::Zero());
    }
    if (hasNormals) {
        normalId.push_back(Vector3d32::Zero());
    }
}

void Mesh3DDecorated::addAOB(const AxisAlignedBoxParameters &box, bool addFaces)
{
    Mesh3D::addAOB(box, addFaces);
}

void Mesh3DDecorated::addAOB(const AxisAlignedBox3d &box, bool addFaces)
{
    Mesh3D::addAOB(box, addFaces);
}

void Mesh3DDecorated::addAOB(const Vector3dd &c1, const Vector3dd &c2, bool addFaces)
{
    Mesh3D::addAOB(c1, c2, addFaces);

  /*  textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));

    textureCoords.push_back(Vector2dd(0.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,0.0));
    textureCoords.push_back(Vector2dd(1.0,1.0));
    textureCoords.push_back(Vector2dd(0.0,1.0));*/

}

void Mesh3DDecorated::addTriangleT(const Vector3dd &p1, const Vector2dd &t1, const Vector3dd &p2, const Vector2dd &t2, const Vector3dd &p3, const Vector2dd &t3)
{
    addTriangle(p1, p2, p3);
    if (hasTexCoords)
    {
        int texStart = (int)textureCoords.size();
        textureCoords.push_back(t1);
        textureCoords.push_back(t2);
        textureCoords.push_back(t3);

        /* addTriangle already preallocated texId*/
        texId.back() = Vector4d32(texStart, texStart + 1, texStart + 2, (int)currentTexture);
    }
    if (hasNormals)
    {
        int normals = (int)normalCoords.size() - 1;
        /* addTriangle already preallocated normalId*/
        normalId.back() = Vector3d32(normals, normals, normals);
    }
}

void Mesh3DDecorated::add(const Mesh3D &other, bool preserveColor)
{
    //SYNC_PRINT(("Mesh3DDecorated::add(const Mesh3D): called\n"));
    Mesh3D::add(other, preserveColor);
}

void Mesh3DDecorated::add(const Mesh3DDecorated &other, bool preserveColor)
{
    // SYNC_PRINT(("Mesh3DDecorated::add(const Mesh3DDecorated): called\n"));

    size_t newTexIdZero    = texId.size();
    size_t newNormalIdZero = normalId.size();


    size_t newTexZero    = textureCoords.size();
    size_t newNormalZero = normalCoords.size();

    Vector4d32 startTexId    = Vector4d32(newTexZero, newTexZero, newTexZero, 0);
    Vector3d32 startNormalId = Vector3d32(newNormalZero, newNormalZero, newNormalZero);

    texId   .reserve(texId   .size() + other.texId.size());
    normalId.reserve(normalId.size() + other.normalId.size());
    /** This would allocate and fill texId and normalId with default values **/
    Mesh3D::add((const Mesh3D &)other, preserveColor);

    if (hasTexCoords) {
        if (other.hasTexCoords) {
            textureCoords.reserve(textureCoords.size() + other.textureCoords.size());
            /* Could copy all together */
            for (size_t i = 0; i < other.textureCoords.size(); i++) {
                textureCoords.push_back(other.textureCoords[i]);
            }
            for (size_t i = 0; i < other.texId.size(); i++) {
                texId[newTexIdZero + i] = other.texId[i] +  startTexId;
            }
        }
    }

    if (hasNormals) {
        if (other.hasNormals) {
            normalCoords.reserve(normalCoords.size() + other.normalCoords.size());

            for (size_t i = 0; i < other.normalCoords.size(); i++)
            {
                normalCoords.push_back(other.normalCoords[i]);
            }

            for (size_t i = 0; i < other.normalId.size(); i++)
            {
                normalId[i + newNormalIdZero] = other.normalId[i] + startNormalId;
            }
        }
    }
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
    out << " Normals  On:"  << (hasNormals ? "on" : "off") << endl;
    out << " Textures On:"  << (hasTexCoords ? "on" : "off") << endl;

    out << " - Normals   :" << normalCoords.size() << endl;
    out << " - Textures  :" << textureCoords.size() << endl;
    out << " - Norm Idxes:" << normalId.size() << endl;
    out << " - Tex  Idxes:" << texId.size() << endl;

    out << " Materials      :" << materials.size() << endl;

    /* */
    std::set<int> usedMaterials;
    for (size_t faceNum = 0; faceNum < faces.size(); faceNum++)
    {
        int material = texId[faceNum][Mesh3DDecorated::MATERIAL_NUM];
        usedMaterials.insert(material);
    }
    out << " Materials Used :" << usedMaterials.size() << endl;

}

void Mesh3DDecorated::fillTestScene()
{
    Mesh3D::fillTestScene();

    /* Normals */
    recomputeMeanNormals();

    /* Texture coordinates */
    textureCoords.push_back(Vector2dd(0.0, 0.0));
    textureCoords.push_back(Vector2dd(0.0, 1.0));
    textureCoords.push_back(Vector2dd(1.0, 0.0));

    for (size_t face = 0; face < faces.size(); face++ )
    {
        texId.push_back(Vector4d32(0, 1, 2, 0));
    }
    hasTexCoords = true;
}

void Mesh3DDecorated::recomputeMeanNormals()
{
    switchNormals(true);

    normalCoords.clear();
    normalId.clear();

    /* We don't need this, just normalise */
    //vector<int> counts;
    normalCoords.resize(vertexes.size(), Vector3dd::Zero());
    //counts.resize(vertexes.size(), 0 );

    for (size_t f = 0; f < faces.size(); f++)
    {
        Triangle3dd triangle = getFaceAsTrinagle(f);
        Vector3dd normal = triangle.getNormal();
        for (int c = 0; c < 3; c++) {
            int vertexId = faces[f][c];
            normalCoords[vertexId] += normal;
            //counts[vertexId] ++;
        }
        normalId.push_back(faces[f]);
    }

    for (size_t n = 0; n < normalCoords.size(); n++)
    {
        //normalCoords[n] /= counts[n];
        normalCoords[n].normalise();
    }
}

bool Mesh3DDecorated::verify( void )
{
    if (hasTexCoords) {
        if (faces.size() != texId.size())
        {
            SYNC_PRINT(("Wrong texId number (faces.size() %d != texId.size() %d)\n", (int)faces.size(), (int)texId.size()));
            return false;
        }
    }

    if (hasNormals) {
        if (faces.size() != normalId.size())
        {
            SYNC_PRINT(("Wrong normalId index number (%d != %d)\n", (int)faces.size(), (int)normalId.size()));
            return false;
        }
    }

    for (size_t i = 0; i < faces.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (faces[i][j] < 0)
            {
                SYNC_PRINT(("Wrong face index (negative) for face %d point %d vertex %d\n", (int)i, j, faces[i][j]));
                return false;
            }

            if (faces[i][j] >= (int)vertexes.size() ) {
                SYNC_PRINT(("Wrong face index for face %d point %d vertex %d (of %d)\n", (int)i, j, faces[i][j], (int)vertexes.size()));
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
            if (texId[i][3] < 0 || texId[i][3] >= (int)materials.size())
            {
                SYNC_PRINT(("Mesh3DDecorated::verify(): Wrong texture name (%d of %d)\n", texId[i][3], (int)materials.size()));
                return false;
            }
        }
    }

    for (size_t i = 0; i < normalId.size(); i++) {
        for (int j = 0; j < 3; j++) {
            if (normalId[i][j] > (int)normalCoords.size() && normalId[i][j] != -1) {
                SYNC_PRINT(("Wrong normal index for face %" PRISIZE_T " - [%d %d %d]\n",
                     i, normalId[i][0], normalId[i][1], normalId[i][2]));
                return false;
            }
        }
    }
    return true;
}



} // namespace corecvs

