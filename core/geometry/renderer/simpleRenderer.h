#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

/**
 *  \file  simpleRenderer.h
 **/
#include <vector>

#include "core/utils/global.h"
#include "core/geometry/renderer/geometryIterator.h"
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/mesh3DDecorated.h"
#include "core/buffers/mipmapPyramid.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/renderer/attributedTriangleSpanIterator.h"
#include "core/fileformats/bmpLoader.h"


namespace corecvs {

class Mesh3D;
class RGB24Buffer;

class SimpleRenderer
{
public:
    SimpleRenderer();
    Matrix44 modelviewMatrix;

    bool drawFaces;
    bool drawEdges;
    bool drawVertexes;


    AbstractBuffer<double> *zBuffer;
    void render (Mesh3D *mesh, RGB24Buffer *buffer);

    /* Add support for face and vertex shaders */
    void fragmentShader(AttributedHLineSpan & span);

    ~SimpleRenderer();
};


class ClassicRenderer
{
public:
    ClassicRenderer();

    enum {
        ATTR_INV_Z = 0 ,
        ATTR_NORMAL_X = 1,
        ATTR_NORMAL_Y,
        ATTR_NORMAL_Z,
        ATTR_TEX_U = 4,
        ATTR_TEX_V,
        ATTR_TEX_ID,

        ATTR_TEX_DU_DY,
        ATTR_TEX_DV_DY,

        ATTR_LAST


    };

    bool drawFaces = true;
    bool drawVertexes = false;
    bool useMipmap = false;

    //bool trueTexture; // it is now always true;
    Matrix44 modelviewMatrix;

protected:
    vector<RGB24Buffer *> textures;
    vector<RGB24Buffer *> midmap;

public:
    void addTexture (RGB24Buffer *buffer, bool produceMidmap = false);

    AbstractBuffer<double> *zBuffer;

    void render (Mesh3DDecorated *mesh, RGB24Buffer *buffer);

    /* These are shader uniform vars*/
    RGB24Buffer *cBuffer;

    /*Debug buffers*/
    AbstractBuffer<double> *scaleDebug = NULL;

    /* */
    RGBColor color;

    virtual void fragmentShader(AttributedHLineSpan & span);
    virtual ~ClassicRenderer();
};

} // namespace corecvs

#endif // SIMPLERENDERER_H
