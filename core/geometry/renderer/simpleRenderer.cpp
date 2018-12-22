#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/geometry/renderer/attributedTriangleSpanIterator.h"
#include <math.h>

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
                AttributedHLineSpan span = it.getAttrSpan();
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

void SimpleRenderer::fragmentShader(AttributedHLineSpan &/*span*/)
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

    if (drawFaces)
    { 
        for(size_t f = 0; f < mesh->faces.size(); f++)
        {
            // printf("\n\n\n ------!! %i !!------ \n\n\n", f);
            Vector3d32 face = mesh->faces[f];
            Vector3d32 normalId = mesh->normalId[f];
            Vector4d32 textureId = mesh->texId[f];

            bool hasNormal = (normalId[0] != -1) && (normalId[1] != -1) && (normalId[2] != -1);
            /**/
            Vector3dd positions[3];
            Vector3dd normals  [3];
            Vector2dd texture  [3];
            double texId = textureId[3];

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
                p.attributes.push_back(texId);

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
            // Triangle is above viewport
            if (it.sortedt.p3().y() < 0)
                continue;

            // Skip lines untill the first that fits in the buffer
            if (it.sortedt.p1().y() < 0 && it.hasValue())
                it.stepTo(0);

            while (it.hasValue())
            {
                AttributedHLineSpan span = it.getAttrSpan();
                span.catt.push_back(it.part.a1[4]);
                span.catt.push_back(it.part.a1[5]);
                span.datt.push_back((it.part.a2[4] - it.part.a1[4]) / (span.x2 - span.x1) );
                span.datt.push_back((it.part.a2[5] - it.part.a1[5]) / (span.x2 - span.x1) );
                
                fragmentShader(span);
                it.step();
            }
        }
    }

    if (drawVertexes)
    {
        for(size_t v = 0; v < mesh->vertexes.size(); v++)
        {
             Vector3dd position = modelviewMatrix * mesh->vertexes[v];
             AttributedHLineSpan span;
             span.x1 = fround(position.x());
             span.x2 = fround(position.x());
             span.cy = fround(position.y());

             double invz = 1.0 / position.z();
             span.catt.push_back(invz);

             span.catt.push_back(0.0);
             span.catt.push_back(0.0);
             span.catt.push_back(0.0);

             if (!trueTexture) {
                 span.catt.push_back(0.0);
                 span.catt.push_back(0.0);
             } else {
                 span.catt.push_back(0.0);
                 span.catt.push_back(0.0);
             }
             span.catt.push_back(0);

             fragmentShader(span);

        }
    }
}

void ClassicRenderer::fragmentShader(AttributedHLineSpan &span)
{
    double lastx = 0.0, lasty = 0.0; // убрать после отладки
    if (span.hasValue() && span.x() < 0)
        span.stepTo(0);

    while (span.hasValue())
    {
        if (cBuffer->isValidCoord(span.pos()) )
        {
            const FragmentAttributes &att = span.att();
            double z = 1.0 / att[0];

            Vector3dd normal = Vector3dd(att[1], att[2], att[3]).normalised();
            Vector2dd tex = Vector2dd(att[4], att[5]);
            Vector2dd dhatt = Vector2dd(span.datt[4], span.datt[5]);
            Vector2dd dvatt = Vector2dd(span.catt[4], span.catt[5]);
 
            int texId = att[6];

            if (trueTexture) {
                tex *= z;
                dhatt*= z;
                dvatt *= z;
            }

            

            tex.y() = 1 - tex.y();


            if (zBuffer->element(span.pos()) > z)
            {
                zBuffer->element(span.pos()) = z;

                RGBColor c = color;
                
                /*
                printf("\n||tex x %f, tex y %f ||\n",tex.x(), tex.y());
                printf("\n|| x %f, %f ||\n",dhatt[0], dvatt[0]);
                printf("\n|| y %f, %f ||\n",dhatt[1], dvatt[1]);
                printf("\n|| x %f, y %f ||\n",tex.x() -lastx, tex.y() - lasty);
                */
                
                lastx = tex.x();  // <---
                lasty = tex.y(); // <---
                
                if (texId < (int)textures.size() && textures[texId] != NULL)
                {
                    RGB24Buffer *texture = textures[texId];
                    if(useMipmap){
                    
                        texId = (int)midmap.size() - 1;
                        // printf("\n|| 000 texid %i ||\n",texId);

                        for (int i = 0; i < (int)midmap.size() - 1; i++){
                            if ((1.0 / dhatt[0]) > midmap[i+1]->h ){
                                texId = i;
                                // printf("\n|| 111 texid %i ||\n",texId);
                                break;
                                
                            }
                        }
                        texture = midmap[texId];
                        double factor = log(abs(1.0 / dhatt[0]))/log(2);
                        factor = factor - trunc(factor);
                        // printf("\n|| factor %f ||\n",factor);

                        // printf("\n|| texid %i ||\n",texId);
                        RGB24Buffer *texturemip = new RGB24Buffer(midmap[texId]->w, midmap[texId]->h);

                        if ((texId != (int)midmap.size() - 1) && ((1 / dhatt[0]) < midmap[0]->h )){    
                            
                            for (int32_t i = 1; i <= midmap[texId + 1]->h; i++)
                            {
                                for (int32_t j = 1; j <= midmap[texId + 1]->w; j++)
                                {
                                    texturemip->element(i*2-2,j*2-2) = midmap[texId]->element(i*2-2,j*2-2) * (factor) + midmap[texId + 1]->element(i-1,j-1) * (1 - factor);
                                    texturemip->element(i*2-2,j*2-1) = midmap[texId]->element(i*2-2,j*2-1) * (factor) + midmap[texId + 1]->element(i-1,j-1) * (1 - factor);
                                    texturemip->element(i*2-1,j*2-2) = midmap[texId]->element(i*2-1,j*2-2) * (factor) + midmap[texId + 1]->element(i-1,j-1) * (1 - factor);
                                    texturemip->element(i*2-1,j*2-1) = midmap[texId]->element(i*2-1,j*2-1) * (factor) + midmap[texId + 1]->element(i-1,j-1) * (1 - factor);
                                }
                            }
                            tex = tex * Vector2dd(texturemip->w, texturemip->h);
                            if (texturemip->isValidCoordBl(tex)) {
                                // printf("\ncase1\n");
                                c = texturemip->elementBl(tex);
                            } else {
                                SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                            }
                            
                        } else {
                            tex = tex * Vector2dd(texture->w, texture->h);
                            if (texture->isValidCoordBl(tex)) {
                                // printf("\ncase2\n");
                                c = texture->elementBl(tex);
                            } else {
                                SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                            }
                        }

                        delete texturemip;                   

                    } else {

                        tex = tex * Vector2dd(texture->w, texture->h);
                        if (texture->isValidCoordBl(tex)) {
                            // printf("\ncase3\n");
                            c = texture->elementBl(tex);
                        } else {
                            SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                        }

                    }

                } else {
                    SYNC_PRINT(("Tex is NULL or non-existent %d\n", texId));
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
