#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

/**
 *  \file  simpleRenderer.h
 **/
#include <vector>

#include "utils/global.h"
#include "geometry/renderer/geometryIterator.h"
#include "buffers/abstractBuffer.h"
#include "buffers/rgb24/abstractPainter.h"
#include "geometry/mesh/mesh3DDecorated.h"
#include "buffers/mipmapPyramid.h"
#include "buffers/rgb24/rgb24Buffer.h"
#include "geometry/renderer/attributedTriangleSpanIterator.h"
#include "fileformats/bmpLoader.h"
#include "utils/debuggableBlock.h"


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
    bool trace = false;


    AbstractBuffer<double> *zBuffer;
    void render (Mesh3D *mesh, RGB24Buffer *buffer);

    /* Add support for face and vertex shaders */
    void fragmentShader(AttributedHLineSpan & span);

    ~SimpleRenderer();
};


class ClassicRenderer : public DebuggableBlock
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
    vector<vector<RGB24Buffer *>> midmap;
    int num_of_textures = 0;
public:
    void addTexture (RGB24Buffer *buffer, bool produceMidmap = false);

    AbstractBuffer<double> *zBuffer;

    void render (Mesh3DDecorated *mesh, RGB24Buffer *buffer);

    /* These are shader uniform vars*/
    RGB24Buffer *cBuffer;

    /* */
    RGBColor color;

    virtual void fragmentShader(AttributedHLineSpan & span);
    virtual ~ClassicRenderer();

    /*Debug buffers*/
    AbstractBuffer<double> *scaleDebug  = NULL;
    AbstractBuffer<double> *factorDebug = NULL;
    AbstractBuffer<double> *levelDebug  = NULL;

    AbstractBuffer<double> *hdxDebug = NULL;
    AbstractBuffer<double> *hdyDebug = NULL;

    AbstractBuffer<double> *vdxDebug = NULL;
    AbstractBuffer<double> *vdyDebug = NULL;

    virtual std::vector<std::string> debugBuffers() const override;
    virtual RGB24Buffer *getDebugBuffer(const std::string& name) const override;


};

} // namespace corecvs

#endif // SIMPLERENDERER_H
