#ifndef MESH3DDECORATED_H
#define MESH3DDECORATED_H

#include "mesh3d.h"

namespace corecvs
{

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


    Mesh3DDecorated();

    void switchTextures(bool on = true);
    void switchNormals(bool on = true);

    virtual void addAOB(const Vector3dd &corner1, const Vector3dd &corner2, bool addFaces = true) override;

    virtual void transform(const Matrix44 &matrix) override;

    virtual void clear();

    virtual void dumpInfo(ostream &out) override;

    bool verify( void );
};

} // namespace corecvs

#endif // MESH3DDECORATED_H
