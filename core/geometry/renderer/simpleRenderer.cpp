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

            if (trace) {
                cout << "SimpleRenderer::render(): " << mesh->vertexes[p] << " => " << position  << endl;
            }

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
   zBuffer(NULL)
{

}

void ClassicRenderer::addTexture(RGB24Buffer *buffer, bool produceMidmap)
{
    textures.push_back(buffer);
    midmap.resize(num_of_textures + 1);
    if (produceMidmap)
    {
        useMipmap = true;
        int numLevels = trunc(log(buffer->h) / log(2)) + 1;
        AbstractMipmapPyramid<RGB24Buffer> *pyramide = new  AbstractMipmapPyramid<RGB24Buffer>(buffer, numLevels, true);
        for (int i = 0; i < (int)pyramide->levels.size(); i++){
            midmap[num_of_textures].push_back(pyramide->levels[i]);
            // BMPLoader().save(std::string("chess") + std::to_string(i) + ".bmp", pyramide->levels[i]);
        }
    } else {
        midmap[num_of_textures].push_back(NULL);
    }
    num_of_textures++;
}

void ClassicRenderer::render(Mesh3DDecorated *mesh, RGB24Buffer *buffer)
{
    SYNC_PRINT(( "ClassicRenderer::render(): drawFaces    - %s\n", drawFaces    ? "yes" : "no" ));
    SYNC_PRINT(( "ClassicRenderer::render(): drawVertexes - %s\n", drawVertexes ? "yes" : "no" ));
    SYNC_PRINT(( "ClassicRenderer::render(): useMipmap    - %s\n", useMipmap    ? "yes" : "no" ));

    if (mesh == NULL || buffer == NULL)
    {
        return;
    }

    delete_safe(zBuffer);
    zBuffer = new AbstractBuffer<double>(buffer->getSize(), std::numeric_limits<double>::max());
    cBuffer = buffer;

    if (useMipmap) {
        delete_safe(scaleDebug);
        scaleDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(vdxDebug);
        vdxDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(vdyDebug);
        vdyDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(hdxDebug);
        hdxDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(hdyDebug);
        hdyDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(factorDebug);
        factorDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);

        delete_safe(levelDebug);
        levelDebug = new AbstractBuffer<double>(buffer->h, buffer->w, 0.0);
    }

    Matrix44 normalTransform = modelviewMatrix.inverted().transposed();

    if (drawFaces)
    {
        for(size_t f = 0; f < mesh->faces.size(); f++)
        {
            if (f % 100 == 0) {
                SYNC_PRINT(("Processed %d (of %d)\n", (int)f, (int)mesh->faces.size()));
            }

            // printf("\n\n\n ------!! %i !!------ \n\n\n", f);
            Vector3d32 face = mesh->faces[f];

            Vector3d32 normalId(-1);
            if (mesh->hasNormals && (f < mesh->normalId.size())) {
                    normalId = mesh->normalId[f];
            }

            Vector4d32 textureId(-1);
            if (mesh->hasTexCoords && (f < mesh->texId.size())) {
                    textureId = mesh->texId[f];
            }

            bool hasNormal = (normalId[0] != -1) && (normalId[1] != -1) && (normalId[2] != -1);
            /**/
            Vector4dd positions[3];
            Vector3dd normals  [3];
            Vector2dd texture  [3];
            double texId = textureId[3];

            bool behindPoint = false;

            AttributedTriangle triang;

            for (int i = 0; i < 3; i++) {
                Vector4dd worldPos(mesh->vertexes[face[i]], 1.0);

                positions[i] = modelviewMatrix * worldPos;
                if (hasNormal) {
                    normals[i] = normalTransform * mesh->normalCoords[normalId[i]].normalised();
                } else {
                    normals[i] = mesh->getFaceAsTrinagle(f).getNormal();
                }

                if (positions[i].z() <= 0) {
                    behindPoint = true;
                    break;
                }

                if (textureId.x() != -1 && textureId.y() != -1 && textureId.z() != -1)
                {
                    texture[i] = mesh->textureCoords[textureId[i]];
                } else {
                    texture[i] = Vector2dd::Zero();
                }

                AttributedPoint &p = triang.p[i];

                p = positions[i].xy() / positions[i].z();
                double invz = 1.0 / positions[i].z();

                p.attributes.resize(ATTR_LAST);
                p.attributes[ATTR_INV_Z] = invz;

                p.attributes[ATTR_NORMAL_X] = normals[i].x();
                p.attributes[ATTR_NORMAL_Y] = normals[i].y();
                p.attributes[ATTR_NORMAL_Z] = normals[i].z();

                p.attributes[ATTR_TEX_U] = texture[i].x() * invz;
                p.attributes[ATTR_TEX_V] = texture[i].y() * invz;

                p.attributes[ATTR_TEX_ID] = texId;

    #if 0
                cout << "P" << i << " :";
                for (int ac = 0; ac < p.attributes.size(); ac++)
                {
                    cout << p.attributes[ac] << " ";
                }
                cout << endl;
    #endif
            }

            if (behindPoint) {
                SYNC_PRINT(("At least one point of Triangle %d is behind camera\n", (int)f));
                continue;
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
                if (span.y() >= cBuffer->h)
                    break;

                // We are adding to current line iterator data from the texture attribute for the next line
                // We are interested in the increment of u and v over line
                double dx1 = it.part.dx1;
                double dx2 = it.part.dx2;
                double ns  = ((it.part.x2 + dx2) - (it.part.x1 + dx1));

//                cout << "dx1  " << dx1 << endl;
//                cout << "dx2  " << dx2 << endl;
//                cout << " s   " << (it.part.x2 - it.part.x1) << endl;
//                cout << "ns   " << ns << endl;


                /* Texture coordinates at the ends next span */
                double tx1 = it.part.a1[ATTR_TEX_U] + it.part.da1[ATTR_TEX_U];
                double tx2 = it.part.a2[ATTR_TEX_U] + it.part.da2[ATTR_TEX_U];

//                cout << "tx1c " << it.part.a1[ATTR_TEX_U] << endl;
//                cout << "tx2c " << it.part.a2[ATTR_TEX_U] << endl;
//                cout << "tx1  " << tx1 << endl;
//                cout << "tx2  " << tx2 << endl;


                /* Texture coordinates at the next span directly lower then current span */
                double nx1 = (tx2 - tx1) / ns * -dx1       + tx1;
                double nx2 = (tx2 - tx1) / ns * (ns - dx2) + tx1;


//                cout << "nx1  " << nx1 << endl;
//                cout << "nx2  " << nx2 << endl;


                nx1 = nx1 - it.part.a1[ATTR_TEX_U];
                nx2 = nx2 - it.part.a2[ATTR_TEX_U];


//                cout << "fnx1 " << nx1 << endl;
//                cout << "fnx2 " << nx2 << endl;


                span.catt[ATTR_TEX_DU_DY] =        nx1;
                span.datt[ATTR_TEX_DU_DY] = (nx2 - nx1) / (span.x2 - span.x1);


                span.catt[ATTR_TEX_DV_DY] =                            it.part.da1[ATTR_TEX_V];
                span.datt[ATTR_TEX_DV_DY] = (it.part.da2[ATTR_TEX_V] - it.part.da1[ATTR_TEX_V]) / (span.x2 - span.x1);
                
//                cout << "New Span " << span.x2 << " " << span.x1 << " len "  << (span.x2 - span.x1) << endl;
//                cout << "delta at left  " << it.part.a1[ATTR_TEX_U] << " " << it.part.da1[ATTR_TEX_U] << endl;
//                cout << "delta at right " << it.part.a2[ATTR_TEX_U] << " " << it.part.da2[ATTR_TEX_U] << endl;

//                cout << "delta at left  " << it.part.a1[ATTR_TEX_V] << " " << it.part.da1[ATTR_TEX_V] << endl;
//                cout << "delta at right " << it.part.a2[ATTR_TEX_V] << " " << it.part.da2[ATTR_TEX_V] << endl;

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

             span.catt.resize(ATTR_LAST);

             double invz = 1.0 / position.z();
             span.catt[ATTR_INV_Z] = invz;

             span.catt[ATTR_NORMAL_X] = 0.0;
             span.catt[ATTR_NORMAL_Y] = 0.0;
             span.catt[ATTR_NORMAL_Z] = 0.0;

             span.catt[ATTR_TEX_U] = 0.0;
             span.catt[ATTR_TEX_V] = 0.0;

             span.catt[ATTR_TEX_ID] = 0.0;

             fragmentShader(span);

        }
    }
}

void ClassicRenderer::fragmentShader(AttributedHLineSpan &span)
{
    if (span.hasValue() && span.x() < 0)
        span.stepTo(0);

    while (span.hasValue() && span.x() < cBuffer->w)
    {
        const FragmentAttributes &att = span.att();

        if (cBuffer->isValidCoord(span.pos()) || att[ATTR_INV_Z] <= 0)
        {

            double z = 1.0 / att[ATTR_INV_Z];

            Vector3dd normal = Vector3dd(att[ATTR_NORMAL_X], att[ATTR_NORMAL_Y], att[ATTR_NORMAL_Z]).normalised();
            Vector2dd tex    = Vector2dd(att[ATTR_TEX_U], att[ATTR_TEX_V]);
            Vector2dd texfar = Vector2dd(att[ATTR_TEX_U], att[ATTR_TEX_V]);

            Vector2dd dhatt = Vector2dd(span.datt[ATTR_TEX_U], span.datt[ATTR_TEX_V]); // Delta stores the increment in horisontal direction
            Vector2dd dvatt = Vector2dd(att[ATTR_TEX_DU_DY], att[ATTR_TEX_DV_DY]);
 

            int texId = att[ATTR_TEX_ID];
            int texMipMapId = 0;

            /* Texture should be mapped linear, but not in the image space, but in 3D space. This is a reason for this projective correction */
            tex *= z;
            texfar *= z;
            dhatt *= z;
            dvatt *= z;

            tex.y() = 1 - tex.y();
            texfar.y() = 1 - texfar.y();


            if (zBuffer->element(span.pos()) > z)
            {
                zBuffer->element(span.pos()) = z;


                RGBColor c = color;
                
                if (texId >= 0 && texId < (int)textures.size() && textures[texId] != NULL)
                {                    
                    if(useMipmap)
                    {

                        vdxDebug->element(span.pos()) = dvatt.x();
                        vdyDebug->element(span.pos()) = dvatt.y();

                        hdxDebug->element(span.pos()) = dhatt.x();
                        hdyDebug->element(span.pos()) = dhatt.y();

                        double scale = sqrt(dhatt.sumAllElementsSq() + dvatt.sumAllElementsSq());
                        scaleDebug->element(span.pos()) = scale;

                        double factor = 0.0;
                        double p1 = sqrt(dhatt[0]*dhatt[0] + dhatt[1]*dhatt[1]);
                        double p2 = sqrt(dvatt[0]*dvatt[0] + dvatt[1]*dvatt[1]);
                        double pmax = p1 >= p2 ? p1 : p2;
                        double pmin = p1 <= p2 ? p1 : p2;
                        double n = pmax/pmin <= (int)midmap[texId].size() - 1 ? pmax/pmin : (int)midmap[texId].size() - 1;
                        double lambda = (log(pmax / n) / log(2)) + 7;
                        
                        if (lambda < 0) {
                            texMipMapId = 0;
                            factor = 1;
                        } else if (lambda > (int)midmap[texId].size() - 1){
                            texMipMapId = (int)midmap[texId].size() - 1;
                            factor = 1;
                        } else{
                            texMipMapId = trunc(lambda);
                            factor = 1 - (lambda - texMipMapId);
                        }
                        if (lambda != lambda) texMipMapId = 0;       

                        RGB24Buffer *texture = midmap[texId][texMipMapId];

                        if ((texMipMapId != (int)midmap[texId].size() - 1)){   

                            factorDebug->element(span.pos()) = factor;
                            levelDebug ->element(span.pos()) = texMipMapId;

                            RGB24Buffer *texturefar = midmap[texId][texMipMapId + 1];

                            tex    = tex    * Vector2dd(texture->w - 1, texture->h - 1);
                            texfar = texfar * Vector2dd(texturefar->w - 1, texturefar->h - 1);

                            if (texture->isValidCoordBl(tex) && texturefar->isValidCoordBl(texfar)) {
                                RGBColor near = texture->elementBl(tex);
                                RGBColor far = texturefar->elementBl(texfar);
                                c = near * factor + far * (1 - factor);
                            } else {
                                SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                            }  
                        } else {
                            tex = tex * Vector2dd(texture->w, texture->h);
                            if (texture->isValidCoordBl(tex)) {
                                c = texture->elementBl(tex);
                            } else {
                                SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                            }
                        }                  
                    } else {
                        RGB24Buffer *texture = textures[texId];
                        tex = tex * Vector2dd(texture->w, texture->h);
                        if (texture->isValidCoordBl(tex)) {
                            c = texture->elementBl(tex);
                        } else {
                            SYNC_PRINT(("Tex miss %lf %lf\n", tex.x(), tex.y()));
                        }

                    }

                } else {
                    static bool suppress = false;
                    if (!suppress) {
                        SYNC_PRINT(("Tex is NULL or non-existent %d\n", texId));
                        suppress = true;
                    }
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

std::vector<std::string> ClassicRenderer::debugBuffers() const
{
    std::vector<std::string> toReturn;
    if (factorDebug != NULL) toReturn.push_back("factor");
    if (levelDebug  != NULL) toReturn.push_back("level");

    if (scaleDebug != NULL) toReturn.push_back("scale");
    if (vdxDebug != NULL)   toReturn.push_back("vdx");
    if (vdyDebug != NULL)   toReturn.push_back("vdy");

    if (hdxDebug != NULL)   toReturn.push_back("hdx");
    if (hdyDebug != NULL)   toReturn.push_back("hdy");

    return toReturn;
}

RGB24Buffer *ClassicRenderer::getDebugBuffer(const std::string &name) const
{
    RGB24Buffer *toReturn = NULL;
    if (name == "factor") {
        toReturn = new RGB24Buffer(factorDebug->h, factorDebug->w);
        toReturn->drawContinuousBuffer<double>(factorDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "level") {
        toReturn = new RGB24Buffer(levelDebug->h, levelDebug->w);
        toReturn->drawContinuousBuffer<double>(levelDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "scale") {
        toReturn = new RGB24Buffer(scaleDebug->h, scaleDebug->w);
        toReturn->drawContinuousBuffer<double>(scaleDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "vdx")   {
        toReturn = new RGB24Buffer(vdxDebug->h, vdxDebug->w);
        toReturn->drawContinuousBuffer<double>(vdxDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "vdy")   {
        toReturn = new RGB24Buffer(vdyDebug->h, vdyDebug->w);
        toReturn->drawContinuousBuffer<double>(vdyDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "hdx")   {
        toReturn = new RGB24Buffer(hdxDebug->h, hdxDebug->w);
        toReturn->drawContinuousBuffer<double>(hdxDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    if (name == "hdy")   {
        toReturn = new RGB24Buffer(hdyDebug->h, hdyDebug->w);
        toReturn->drawContinuousBuffer<double>(hdyDebug, RGB24Buffer::STYLE_RAW, ColorPallete::RAINBOW, true);
    }
    return toReturn;
}




} // namespace corecvs
