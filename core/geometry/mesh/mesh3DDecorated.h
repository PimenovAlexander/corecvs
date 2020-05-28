#ifndef MESH3DDECORATED_H
#define MESH3DDECORATED_H

#include "geometry/mesh/mesh3d.h"
#include "buffers/rgb24/rgb24Buffer.h"

namespace corecvs
{

class OBJMaterial {
public:
    std::string name;

    enum  {
        KOEF_AMBIENT,
        KOEF_DIFFUSE,
        KOEF_SPECULAR,
        KOEF_TRANSMISSION_FILTER,
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

    /**
     * Texture coordinates array. Texture coordinates are referenced by number in this array.
     * This is done to reuse repeating coordinates
     **/
    vector<Vector2dd>   textureCoords;


    /** NormalCoords stores normals directions. They referenced by number in this array */
    vector<Vector3dd>   normalCoords;

    /* Per face ids*/
    /**
     * texId is an array of <Vector4d32>, the size of this array is equal to the size of the array of fases,
     * first three numbers are indexes in textureCoords array, and the fourth number is usually used as texture nunber
     **/
    static const int TEXTURE_NUM = 3;
    static const int MATERIAL_NUM = 3;

    vector<Vector4d32>  texId;  

    vector<Vector3d32>  normalId;

    /* We are not sure what we expect from material.
     * We will use wavefront so far */
    vector<OBJMaterial> materials;

    /* */
    size_t currentTexture = 0;

    Mesh3DDecorated();

    void switchTextures(bool on = true);
    void switchNormals(bool on = true);

    virtual void addFace(const Vector3d32 &faceId) override;

    virtual void addAOB(const AxisAlignedBoxParameters &box, bool addFaces = true) override;
    virtual void addAOB(const AxisAlignedBox3d &box, bool addFaces = true) override;
    virtual void addAOB(const Vector3dd &corner1, const Vector3dd &corner2, bool addFaces = true) override;

    void addTriangleT(const Vector3dd &p1, const Vector2dd &t1,
                      const Vector3dd &p2, const Vector2dd &t2,
                      const Vector3dd &p3, const Vector2dd &t3);

    // using Mesh3D::add;
    virtual void add(const Mesh3D &other, bool preserveColor = false) override;
    virtual void add(const Mesh3DDecorated &other, bool preserveColor = false);

    virtual void transform(const Matrix44 &matrix) override;

    virtual void clear() override;

    virtual void dumpInfo(ostream &out = std::cout) override;

    virtual void fillTestScene() override;

    void recomputeMeanNormals();

    bool verify( void );
};



} // namespace corecvs

#endif // MESH3DDECORATED_H
