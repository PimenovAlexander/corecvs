#ifndef SIMPLERENDERER_H
#define SIMPLERENDERER_H

/**
 *  \file  simpleRenderer.h
 **/

#include "core/geometry/renderer/geometryIterator.h"
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/mesh3DDecorated.h"


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
    bool trueTexture;
    Matrix44 modelviewMatrix;
    vector<RGB24Buffer *> textures;

    AbstractBuffer<double> *zBuffer;

    void render (Mesh3DDecorated *mesh, RGB24Buffer *buffer);

    /* These are shader uniform vars*/
    RGB24Buffer *cBuffer;

    /* */
    RGBColor color;

    virtual void fragmentShader(AttributedHLineSpan & span);
    virtual ~ClassicRenderer();
};

} // namespace corecvs

#endif // SIMPLERENDERER_H
