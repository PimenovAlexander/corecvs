#ifndef MESH3DDECORATED_H
#define MESH3DDECORATED_H

#include "core/geometry/mesh3d.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

namespace corecvs
{

class OBJMaterial {
public:
    enum  {
        KOEF_AMBIENT,
        KOEF_DIFFUSE,
        KOEF_SPECULAR,
        KOEF_LAST
    };

    Vector3dd koefs[KOEF_LAST];

    enum  {
        TEX_AMBIENT,
        TEX_DIFFUSE,
        TEX_SPECULAR,
        TEX_HIGHLIGHT,
        TEX_ALPHA,
        TEX_BUMP,
        TEX_LAST
    };

    RGB24Buffer* tex[TEX_LAST];

    OBJMaterial() {

        for (size_t i = 0; i < CORE_COUNT_OF(koefs); i++)
        {
            koefs[i] = RGBColor::White().toDouble();
        }

        for (size_t j = 0; j < CORE_COUNT_OF(tex); j++)
        {
            tex[j] = NULL;
        }
    }

    friend ostream & operator <<(ostream &out, const OBJMaterial &material)
    {
        for (size_t i = 0; i < CORE_COUNT_OF(material.koefs); i++)
        {
            out << material.koefs[i] << endl;
        }

        for (size_t j = 0; j < CORE_COUNT_OF(material.tex); j++)
        {
            if (material.tex[j] != NULL) {
                out << material.tex[j]->getSize() << endl;
            } else {
                out << "- No -" << endl;
            }
        }
        return out;
    }

};


class Mesh3DDecorated : public Mesh3D
{
public:
    bool hasTexCoords;
    bool hasNormals;

    /* Texture Coords Channel */
    vector<Vector2dd>   textureCoords;
    vector<Vector3dd>   normalCoords;

    /* Per face ids*/
    vector<Vector3d32>  texId;
    vector<Vector3d32>  normalId;

    /* We are not sure what we expect from material.
     * We will use wavefront so far */
    OBJMaterial material;

    Mesh3DDecorated();

    void switchTextures(bool on = true);
    void switchNormals(bool on = true);

    virtual void addAOB(const Vector3dd &corner1, const Vector3dd &corner2, bool addFaces = true) override;

    virtual void transform(const Matrix44 &matrix) override;

    virtual void clear();

    virtual void dumpInfo(ostream &out = std::cout) override;

    virtual void fillTestScene();

    void recomputeMeanNormals();

    bool verify( void );
};

class MeshFilter
{
public:
    static
    void removeDuplicatedFaces(Mesh3DDecorated &mesh);

    static
    void removeUnreferencedVertices(Mesh3DDecorated &mesh);

    static
    void removeIsolatedPieces(Mesh3DDecorated &mesh,  int minCountOfFaces);

    static
    void removeZeroAreaFaces(Mesh3DDecorated &mesh);

   static
    void removeDuplicatedVertices(Mesh3DDecorated &mesh);
};

} // namespace corecvs

#endif // MESH3DDECORATED_H
