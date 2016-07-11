#include "simpleRenderer.h"
#include "mesh3d.h"
#include "rgb24Buffer.h"

namespace corecvs {

SimpleRenderer::SimpleRenderer() :    
    drawFaces(true),
    drawEdges(true),
    drawVertexes(true),
    zBuffer(NULL)
{
}

void SimpleRenderer::render(Mesh3D *mesh, RGB24Buffer *buffer)
{
    if (mesh == NULL || buffer == NULL)
    {
        return;
    }

    delete_safe(zBuffer);
    zBuffer = new AbstractBuffer<double>(buffer->getSize(), std::numeric_limits<double>::max());

    if (drawFaces) {
        for(size_t f = 0; f < mesh->faces.size(); f++)
        {
            Vector3d32 face = mesh->faces[f];

            Vector3dd positions[3];
            AttributedTriangle triang;

            for (int i = 0; i < 3; i++) {
                positions[i] = modelviewMatrix * mesh->vertexes[face[i]];
                triang.p[i] = positions[i].project();
                triang.p[i].attributes.push_back(1.0 / positions[i].z());
            }

            RGBColor color = RGBColor::Blue();
            if (mesh->hasColor) {
                color = mesh->facesColor[f];
            }

            AttributedTriangleSpanIterator it(triang);
            while (it.hasValue())
            {
                it.step();
                AttributedLineSpan span = it.getAttrSpan();
                while (span.hasValue())
                {
                    if (buffer->isValidCoord(span.pos()) )
                    {
                        double z = 1.0 / span.att()[0];
                        if (zBuffer->element(span.pos()) > z)
                        {
                            zBuffer->element(span.pos()) = z;
                            buffer->element(span.pos()) = color;
                        }
                    }
                    span.step();
                }
            }
        }
    }

    if (drawEdges)
    {
        for(size_t e = 0; e < mesh->edges.size(); e++)
        {
            Vector2d32 edge = mesh->edges[e];

            Vector2dd position1 = (modelviewMatrix * mesh->vertexes[edge[0]]).project();
            Vector2dd position2 = (modelviewMatrix * mesh->vertexes[edge[1]]).project();

            RGBColor color = RGBColor::Green();
            if (mesh->hasColor) {
                color = mesh->edgesColor[e];
            }
            buffer->drawLine(position1, position2, color);
        }
    }

    if (drawVertexes)
    {
        for(size_t p = 0; p < mesh->vertexes.size(); p++)
        {
            Vector2dd position = (modelviewMatrix * mesh->vertexes[p]).project();
            cout << "SimpleRenderer::render(): " << mesh->vertexes[p] << " => " << position  << endl;

            RGBColor color = RGBColor::Red();
            if (mesh->hasColor) {
                color = mesh->vertexesColor[p];
            }
            buffer->drawPixel(position.x(), position.y(), color);
        }
    }
}

void SimpleRenderer::fragmentShader(AttributedLineSpan &/*span*/)
{
    /**/
}

SimpleRenderer::~SimpleRenderer()
{
    delete_safe(zBuffer);
}



ClassicRenderer::ClassicRenderer() :
   trueTexture(true),
   zBuffer(NULL)
{

}

void ClassicRenderer::render(Mesh3DDecorated *mesh, RGB24Buffer *buffer)
{
    if (mesh == NULL || buffer == NULL)
    {
        return;
    }

    delete_safe(zBuffer);
    zBuffer = new AbstractBuffer<double>(buffer->getSize(), std::numeric_limits<double>::max());
    cBuffer = buffer;

    Matrix44 normalTransform = modelviewMatrix.inverted().transposed();

    for(size_t f = 0; f < mesh->faces.size(); f++)
    {
        Vector3d32 face = mesh->faces[f];
        Vector3d32 normalId = mesh->normalId[f];
        Vector3d32 textureId = mesh->texId[f];

        bool hasNormal = (normalId[0] != -1) && (normalId[1] != -1) && (normalId[2] != -1);
        /**/
        Vector3dd positions[3];
        Vector3dd normals  [3];
        Vector2dd texture  [3];

        AttributedTriangle triang;

        for (int i = 0; i < 3; i++) {
            positions[i] = modelviewMatrix * mesh->vertexes[face[i]];
            if (hasNormal) {
                normals[i] = /*normalTransform **/ mesh->normalCoords[normalId[i]].normalised();
            } else {
                normals[i] = mesh->getFaceAsTrinagle(f).getNormal();
            }

            texture[i] = mesh->textureCoords[textureId[i]];

            AttributedPoint &p = triang.p[i];

            p = positions[i].project();

            double invz = 1.0 / positions[i].z();
            p.attributes.push_back(invz);

            p.attributes.push_back(normals[i].x());
            p.attributes.push_back(normals[i].y());
            p.attributes.push_back(normals[i].z());

            if (!trueTexture) {
                p.attributes.push_back(texture[i].x());
                p.attributes.push_back(texture[i].y());
            } else {
                p.attributes.push_back(texture[i].x() * invz);
                p.attributes.push_back(texture[i].y() * invz);
            }

#if 0
            cout << "P" << i << " :";
            for (int ac = 0; ac < p.attributes.size(); ac++)
            {
                cout << p.attributes[ac] << " ";
            }
            cout << endl;
#endif
        }

        color = RGBColor::Blue();
        if (mesh->hasColor) {
            color = mesh->facesColor[f];
        }

        AttributedTriangleSpanIterator it(triang);
        while (it.hasValue())
        {
            AttributedLineSpan span = it.getAttrSpan();
            fragmentShader(span);
            it.step();
        }
    }


}

void ClassicRenderer::fragmentShader(AttributedLineSpan &span)
{
    while (span.hasValue())
    {
        if (cBuffer->isValidCoord(span.pos()) )
        {
            const FragmentAttributes &att = span.att();
            double z = 1.0 / att[0];
            Vector3dd normal = Vector3dd(att[1], att[2], att[3]).normalised();
            Vector2dd tex = Vector2dd(att[4], att[5]);
            if (trueTexture) {
                tex *= z;
            }

            tex.y() = 1 - tex.y();


            if (zBuffer->element(span.pos()) > z)
            {
                zBuffer->element(span.pos()) = z;

                RGBColor c = color;

                /* Texture block*/
                RGB24Buffer *texture = textures[0];
                tex = tex * Vector2dd(texture->w, texture->h);
                if (texture->isValidCoordBl(tex)) {
                    c = texture->elementBl(tex);
                } else {
                    SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                }


                /* Normal block*/

                Vector3dd dir = Vector3dd(1.0, 1.0, -1.0).normalised();

                double coef = (normal & dir);
                if (coef < 0) coef = 0.1;
                if (coef > 1.0) coef = 1.0;

//                cout << normal << " " << dir << " " << coef << endl ;
                cBuffer->element(span.pos()) = c;//RGBColor::lerpColor(color, c, coef);



            }
        }
        span.step();
    }

}

ClassicRenderer::~ClassicRenderer()
{

}




} // namespace corecvs
