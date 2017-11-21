#include <qglfunctions.h>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>

//#include <GL/gl.h>

#include <qopengl.h>
#include "sceneShaded.h"
#include "cloudViewDialog.h"
#include "core/fileformats/bmpLoader.h"

QString textGlError(GLenum err)
{
    switch (err) {
    case GL_INVALID_ENUM:      return "GL_INVALID_ENUM"; break;
    case GL_INVALID_VALUE:     return "GL_INVALID_VALUE"; break;
    case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION"; break;
    case GL_STACK_OVERFLOW:    return "GL_STACK_OVERFLOW"; break;
    case GL_STACK_UNDERFLOW:   return "GL_STACK_UNDERFLOW"; break;
    case GL_OUT_OF_MEMORY:     return "GL_OUT_OF_MEMORY"; break;
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION"; break;
    case GL_CONTEXT_LOST:      return "GL_CONTEXT_LOST"; break;
    case GL_TABLE_TOO_LARGE:   return "GL_TABLE_TOO_LARGE"; break;
    default:
        break;
    }
    return "UNKNOWN";
}

void dumpGLErrors()
{
    for(GLenum err; (err = glGetError()) != GL_NO_ERROR;)
    {
       qDebug() << "OpenGL error" << err << " : " << textGlError(err);
    }
}

static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp  vec4 colAttr;\n"
    "attribute lowp  vec4 faceColAttr;\n"
    "attribute lowp  vec4 normalAttr;\n"
    "attribute lowp  vec2 texAttr;\n"
    "varying lowp vec4 col;\n"
    "varying lowp vec4 normal;\n"
    "varying lowp vec2 vTexCoord;\n"
    "uniform highp mat4 modelview;\n"
    "uniform highp mat4 projection;\n"
    "void main() {\n"
    "   col = colAttr / 1.0;\n"
    "   col.a = 1.0;\n"
    "   vTexCoord.x = texAttr.x;"
    "   vTexCoord.y = 1.0 - texAttr.y;\n"
    "   normal = normalAttr;\n"
    "   gl_Position = projection * modelview * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D bumpSampler;\n"
    "varying lowp vec4 col;\n"
    "varying lowp vec2 vTexCoord;\n"
    "varying lowp vec4 normal;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "   gl_FragColor.a = 1.0;\n"
    "   gl_FragDepth = gl_FragCoord.z;\n"
    "}\n";

static const char *vertexShaderSource1 =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp  vec4 colAttr;\n"
    "attribute lowp  vec4 faceColAttr;\n"
    "attribute lowp  vec4 normalAttr;\n"
    "attribute lowp  vec2 texAttr;\n"
    "varying lowp vec4 col;\n"
    "varying lowp vec4 normal;\n"
    "varying lowp vec2 vTexCoord;\n"
    "uniform highp mat4 modelview;\n"
    "uniform highp mat4 projection;\n"
    "void main() {\n"
    "   col = colAttr / 1.0;\n"
    "   col.a = 1.0;\n"
    "   vTexCoord.x = texAttr.x;"
    "   vTexCoord.y = 1.0 - texAttr.y;\n"
    "   normal = normalAttr;\n"
    "   gl_Position = projection * modelview * posAttr;\n"
    "}\n";

static const char *fragmentShaderSource1 =
    "uniform sampler2D textureSampler;\n"
    "uniform sampler2D bumpSampler;\n"
    "varying lowp vec4 col;\n"
    "varying lowp vec2 vTexCoord;\n"
    "varying lowp vec4 normal;\n"
    "void main() {\n"
    "   //gl_FragColor = col;\n"
    "   //gl_FragColor.rg = vTexCoord;\n"
    "   gl_FragColor.rgb  = texture2D(textureSampler, vTexCoord).rgb;\n"
    "   float diff = dot(normal.xyz, vec3(0.5,0.5,0.5));\n"
    "   if (diff < 0.0) diff = 0.0;\n"
    "   gl_FragColor.r += diff;\n"
    "   //gl_FragColor.rgb = normal.xyz;\n"
    "   gl_FragColor.a = 1.0;\n"
    "   gl_FragDepth = gl_FragCoord.z;\n"
    "}\n";


void SceneShaded::addTexture(GLuint texId, RGB24Buffer *input)
{
    //glActiveTexture(0);
    glBindTexture(GL_TEXTURE_2D, texId);
    qDebug() << "Binded texture:" << texId;
    dumpGLErrors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    qDebug() << "Setting params texture:" << texId;
    dumpGLErrors();

    GLint oldStride;
    glGetIntegerv(GL_UNPACK_ROW_LENGTH, &oldStride);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, input->stride);
    qDebug() << "Setted stride: old = " << oldStride << " - " << input->stride;
    dumpGLErrors();
    glTexImage2D(GL_TEXTURE_2D,
             0,
             GL_RGB,
             input->w,
             input->h,
             0,
             GL_BGRA,
             GL_UNSIGNED_BYTE,
             &(input->element(0,0)));
    qDebug() << "Creating texture:" << texId;
    dumpGLErrors();

    glPixelStorei(GL_UNPACK_ROW_LENGTH, oldStride);

    glBindTexture(GL_TEXTURE_2D, 0);
    qDebug() << "Unbinding texture:" << texId;
}

void SceneShaded::setParameters(void *params)
{
    SYNC_PRINT(("SceneShaded::setParameters()\n"));
    mParameters = *static_cast<ShadedSceneControlParameters *>(params);
    mParamsApplied = false;
}

void SceneShaded::applyParameters()
{
    if (mParamsApplied)
        return;

    ShaderPreset *sources[ShaderTarget::LAST] = {
        &mParameters.face,
        &mParameters.point,
        &mParameters.edge
    };

    for (int target = 0; target < ShaderTarget::LAST; target++)
    {
        QString vShader = vertexShaderSource;
        QString fShader = fragmentShaderSource;

        if (sources[target]->type == ShaderPreset::NONE) {
            delete_safe(mProgram[target]);
            continue;
        }

        if(sources[target]->type != ShaderPreset::PRESET) {
            vShader = sources[target]->vertex;
            fShader = sources[target]->fragment;
        }

        if(sources[target]->type == ShaderPreset::PRESET1) {
            vShader = vertexShaderSource1;
            fShader = fragmentShaderSource1;
        }

        SYNC_PRINT(("SceneShaded::setParameters(): Creating %d program\n", target));
        mProgram[target] = new QOpenGLShaderProgram();
        mProgram[target]->addShaderFromSourceCode(QOpenGLShader::Vertex,   vShader);
        mProgram[target]->addShaderFromSourceCode(QOpenGLShader::Fragment, fShader);
        mProgram[target]->link();

        if (!mProgram[target]->isLinked())
        {
            qDebug() << mProgram[target]->log();
        }

        mPosAttr     = mProgram[target]->attributeLocation("posAttr");
        mColAttr     = mProgram[target]->attributeLocation("colAttr");
        mFaceColAttr = mProgram[target]->attributeLocation("faceColAttr");
        mTexAttr     = mProgram[target]->attributeLocation("texAttr");
        mNormalAttr  = mProgram[target]->attributeLocation("normalAttr");

        mModelViewMatrix  = mProgram[target]->uniformLocation("modelview");
        mProjectionMatrix = mProgram[target]->uniformLocation("projection");

        mTextureSampler = mProgram[target]->uniformLocation("textureSampler");
        mBumpSampler    = mProgram[target]->uniformLocation("bumpSampler");

    }
    mParamsApplied = true;
}


void SceneShaded::prepareMesh(CloudViewDialog * /*dialog*/)
{
    SYNC_PRINT(("void SceneShaded::prepareMesh():called\n"));
    initializeOpenGLFunctions();

    setParameters(&mParameters);

    /*Prepare Texture*/
    RGB24Buffer *texBuf = mMesh->material.tex[OBJMaterial::TEX_DIFFUSE];
    if (texBuf != NULL) {
        glEnable(GL_TEXTURE_2D);
        qDebug() << "Dumping prior error";
        dumpGLErrors();
        glGenTextures(1, &mTexture);
        qDebug() << "Created a handle for the texture:" << mTexture;
        dumpGLErrors();
        addTexture(mTexture, texBuf);
        glDisable(GL_TEXTURE_2D);
    }

    /*Prepare Bumpmap*/
    RGB24Buffer *bumpBuf = mMesh->material.tex[OBJMaterial::TEX_DIFFUSE];
    if (bumpBuf != NULL) {
        glEnable(GL_TEXTURE_2D);
        qDebug() << "Dumping prior error";
        dumpGLErrors();
        glGenTextures(1, &mBumpmap);
        qDebug() << "Created a handle for the bumpmap:" << mBumpmap;
        dumpGLErrors();
        addTexture(mBumpmap, bumpBuf);
        glDisable(GL_TEXTURE_2D);
    }

}

void SceneShaded::drawMyself(CloudViewDialog * /*dialog*/)
{
    initializeOpenGLFunctions();
    applyParameters();
/*    if (mProgram[] == NULL)
    {
        qDebug("SceneShaded::drawMyself(): mProgram is NULL");
        return;
    }
    mProgram->bind();*/

    float arr[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, arr);
    QMatrix4x4 modelview(
            arr[0], arr[4], arr[ 8], arr[12],
            arr[1], arr[5], arr[ 9], arr[13],
            arr[2], arr[6], arr[10], arr[14],
            arr[3], arr[7], arr[11], arr[15]
    );

    glGetFloatv(GL_PROJECTION_MATRIX, arr);
    QMatrix4x4 projection(
            arr[0], arr[4], arr[ 8], arr[12],
            arr[1], arr[5], arr[ 9], arr[13],
            arr[2], arr[6], arr[10], arr[14],
            arr[3], arr[7], arr[11], arr[15]
    );

    /*matrix.perspective(60.0f, 4.0f/3.0f, 0.1f, 100.0f);
    matrix.translate(0, 0, -2);*/
   // matrix.rotate(100.0f * m_frame / screen()->refreshRate(), 0, 1, 0);

    for (int target = 0; target < ShaderTarget::LAST; target++)
    {
        if (mProgram[target] == NULL)
            continue;

        mProgram[target]->bind();
        mProgram[target]->setUniformValue(mModelViewMatrix, modelview);
        mProgram[target]->setUniformValue(mProjectionMatrix, projection);
        mProgram[target]->release();
    }

    bool depthTest =  glIsEnabled(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_TEST);

#if 0

    GLfloat vertices[] = {
       0.0f, 0.707f,
       -0.5f, -0.5f,
       0.5f, -0.5f
    };

    GLfloat colors[] = {
       1.0f, 0.0f, 0.0f,
       0.0f, 1.0f, 0.0f,
       0.0f, 0.0f, 1.0f
    };

    glVertexAttribPointer(mPosAttr, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(mColAttr, 3, GL_FLOAT, GL_FALSE, 0, colors);

    glEnableVertexAttribArray(mPosAttr);
    glEnableVertexAttribArray(mColAttr);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableVertexAttribArray(mPosAttr);
    glDisableVertexAttribArray(mColAttr);
#endif

    // qDebug() << "Before Mesh Draw";   dumpGLErrors();

    /* Draw embedded Mesh */
    if (mMesh != NULL)
    {
        /* Draw points */
        if (mParameters.point.type != ShaderPreset::NONE && mProgram[POINT] != NULL)
        {
            mProgram[POINT]->bind();
            int oldPointSize = 1;
            glGetIntegerv(GL_POINT_SIZE, &oldPointSize);
            glPointSize(mParameters.pointSize());

            // cout << "Executing point draw" << endl;
            glVertexAttribPointer(mPosAttr, 3, GL_DOUBLE, GL_FALSE, 0, mMesh->vertexes.data());
            glEnableVertexAttribArray(mPosAttr);

            if (mMesh->hasColor && !mParameters.pointColorOverride())
            {
                glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), mMesh->vertexesColor.data());
                glEnableVertexAttribArray(mColAttr);
            } else {
                Vector3df pointColor = RGBColor(mParameters.pointColor()).toFloat();
                glVertexAttrib3f(mColAttr, pointColor.x(), pointColor.y(), pointColor.z());
            }

            glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mMesh->vertexes.size()));

            glDisableVertexAttribArray(mPosAttr);
            glDisableVertexAttribArray(mColAttr);

            glPointSize(oldPointSize);
            mProgram[POINT]->release();
        }

        if (mParameters.edge.type != ShaderPreset::NONE && mProgram[EDGE] != NULL)
        {
            vector<Vector3dd> positions;
            vector<RGBColor>  vertexColors;
            vector<RGBColor>  edgeColors;
            vector<uint32_t>  edgeIds;

            mProgram[EDGE]->bind();

            RGBColor edgeColor = RGBColor(mParameters.edgeColor());

            int oldLineWidth = 1;
            glGetIntegerv(GL_LINE_WIDTH, &oldLineWidth);
            glLineWidth(mParameters.edgeWidth());

            for (size_t edgeNum = 0; edgeNum < mMesh->edges.size(); edgeNum++)
            {
                Vector2d32 pointId = mMesh->edges[edgeNum];
                positions.push_back(mMesh->vertexes[pointId.x()]);
                positions.push_back(mMesh->vertexes[pointId.y()]);

                if (mMesh->hasColor && !mParameters.edgeColorOverride()) {
                    vertexColors.push_back(mMesh->vertexesColor[pointId.x()]);
                    vertexColors.push_back(mMesh->vertexesColor[pointId.y()]);

                    edgeColors.push_back(mMesh->edgesColor[edgeNum]);
                    edgeColors.push_back(mMesh->edgesColor[edgeNum]);
                } else {
                    vertexColors.push_back(edgeColor);
                    vertexColors.push_back(edgeColor);
                    edgeColors.push_back(edgeColor);
                    edgeColors.push_back(edgeColor);
                }

                edgeIds.push_back((uint32_t)(2 * edgeNum));
                edgeIds.push_back((uint32_t)(2 * edgeNum + 1));
            }

            for (size_t faceNum = 0; faceNum < mMesh->faces.size(); faceNum++)
            {
                Vector3d32 pointId = mMesh->faces[faceNum];
                uint32_t start = (uint32_t)positions.size();

                positions.push_back(mMesh->vertexes[pointId.x()]);
                positions.push_back(mMesh->vertexes[pointId.y()]);
                positions.push_back(mMesh->vertexes[pointId.z()]);

                if (mMesh->hasColor && !mParameters.edgeColorOverride()) {
                    edgeColors.push_back(mMesh->facesColor[faceNum]);
                    edgeColors.push_back(mMesh->facesColor[faceNum]);
                    edgeColors.push_back(mMesh->facesColor[faceNum]);
                } else {
                    edgeColors.push_back(edgeColor);
                    edgeColors.push_back(edgeColor);
                    edgeColors.push_back(edgeColor);
                }

                edgeIds.push_back(start);
                edgeIds.push_back(start + 1);
                edgeIds.push_back(start + 1);
                edgeIds.push_back(start + 2);
                edgeIds.push_back(start + 2);
                edgeIds.push_back(start);
            }

            glVertexAttribPointer(mPosAttr, 3, GL_DOUBLE, GL_FALSE, 0, positions.data());
            glEnableVertexAttribArray(mPosAttr);

            if (!edgeColors.empty())
            {
                glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), edgeColors.data());
                glEnableVertexAttribArray(mColAttr);
            }

            glDrawElements(GL_LINES, GLsizei(edgeIds.size()), GL_UNSIGNED_INT, edgeIds.data());

            glDisableVertexAttribArray(mPosAttr);
            glDisableVertexAttribArray(mColAttr);

            glLineWidth(oldLineWidth);
            mProgram[EDGE]->release();
        }

        /* Draw faces */
        if (mParameters.face.type != ShaderPreset::NONE && mProgram[FACE] != NULL)
        {
            // cout << "Executing face draw" << endl;
            /* Unfotunately we need to unpack all the face related parameters to arrays */
            /* Later we can try to cache this */

            vector<Vector3dd> positions;
            vector<RGBColor>  vertexColors;
            vector<RGBColor>  faceColors;
            vector<Vector3dd> normals;
            vector<Vector2dd> texCoords;
            vector<uint32_t>  faceIds;

            mProgram[FACE]->bind();

            RGBColor faceColor = RGBColor(mParameters.faceColor());

            for (size_t faceNum = 0; faceNum < mMesh->faces.size(); faceNum++)
            {
                Vector3d32 pointId = mMesh->faces[faceNum];

                positions.push_back(mMesh->vertexes[pointId.x()]);
                positions.push_back(mMesh->vertexes[pointId.y()]);
                positions.push_back(mMesh->vertexes[pointId.z()]);

                if (mMesh->hasColor && !mParameters.faceColorOverride()) {
                    vertexColors.push_back(mMesh->vertexesColor[pointId.x()]);
                    vertexColors.push_back(mMesh->vertexesColor[pointId.y()]);
                    vertexColors.push_back(mMesh->vertexesColor[pointId.z()]);

                    faceColors.push_back(mMesh->facesColor[faceNum]);
                    faceColors.push_back(mMesh->facesColor[faceNum]);
                    faceColors.push_back(mMesh->facesColor[faceNum]);
                } else {
                    vertexColors.push_back(faceColor);
                    vertexColors.push_back(faceColor);
                    vertexColors.push_back(faceColor);

                    faceColors.push_back(faceColor);
                    faceColors.push_back(faceColor);
                    faceColors.push_back(faceColor);
                }

                if (mMesh->hasTexCoords)
                {
                    Vector3d32 texId = mMesh->texId[faceNum];
                    texCoords.push_back(mMesh->textureCoords[texId.x()]);
                    texCoords.push_back(mMesh->textureCoords[texId.y()]);
                    texCoords.push_back(mMesh->textureCoords[texId.z()]);
                } else {
                    texCoords.push_back(Vector2dd::Zero());
                    texCoords.push_back(Vector2dd::Zero());
                    texCoords.push_back(Vector2dd::Zero());
                }

                if (mMesh->hasNormals)
                {
                    Vector3d32 normalId = mMesh->normalId[faceNum];
                    normals.push_back(mMesh->normalCoords[normalId.x()]);
                    normals.push_back(mMesh->normalCoords[normalId.y()]);
                    normals.push_back(mMesh->normalCoords[normalId.z()]);
                }

                faceIds.push_back((uint32_t)(3 * faceNum));
                faceIds.push_back((uint32_t)(3 * faceNum + 1));
                faceIds.push_back((uint32_t)(3 * faceNum + 2));
            }

            glVertexAttribPointer(mPosAttr, 3, GL_DOUBLE, GL_FALSE, 0, positions.data());
            glEnableVertexAttribArray(mPosAttr);

            if (!faceColors.empty() && !vertexColors.empty())
            {
                glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), faceColors.data());
                glEnableVertexAttribArray(mColAttr);

                glVertexAttribPointer(mFaceColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), vertexColors.data());
                glEnableVertexAttribArray(mFaceColAttr);

            }
            if (mMesh->hasTexCoords && !texCoords.empty())
            {
                glVertexAttribPointer(mTexAttr, 2, GL_DOUBLE, GL_FALSE, 0, texCoords.data());
                glEnableVertexAttribArray(mTexAttr);
            }

            if (mMesh->hasNormals && !normals.empty())
            {
                glVertexAttribPointer(mNormalAttr, 3, GL_DOUBLE, GL_FALSE, 0, normals.data());
                glEnableVertexAttribArray(mNormalAttr);
            }

            GLboolean oldTexEnable = glIsEnabled(GL_TEXTURE_2D);

#if 1
            if (mMesh->material.tex[OBJMaterial::TEX_DIFFUSE])
            {
                //qDebug() << "Before Texture";   dumpGLErrors();
                glEnable(GL_TEXTURE_2D);
                glActiveTexture(GL_TEXTURE0);
                //qDebug() << "Before Bind";   dumpGLErrors();
                glBindTexture(GL_TEXTURE_2D, mTexture /*dialog->mFancyTexture*/);
                //qDebug() << "Before Sampler";   dumpGLErrors();
                mProgram[FACE]->setUniformValue(mTextureSampler, 0);
                //qDebug() << "Before Call"; dumpGLErrors();
            }

            if (mMesh->material.tex[OBJMaterial::TEX_BUMP])
            {
                glEnable(GL_TEXTURE_2D);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mBumpmap);
                mProgram[FACE]->setUniformValue(mTextureSampler, 1);
            }

            glDrawElements(GL_TRIANGLES, GLsizei(mMesh->faces.size() * 3), GL_UNSIGNED_INT, faceIds.data());
#endif

            glBindTexture(GL_TEXTURE_2D, 0);
            if (!oldTexEnable) {
                glActiveTexture(GL_TEXTURE0);
                glDisable(GL_TEXTURE_2D);

                glActiveTexture(GL_TEXTURE1);
                glDisable(GL_TEXTURE_2D);
            }

            glDisableVertexAttribArray(mPosAttr);
            glDisableVertexAttribArray(mColAttr);
            glDisableVertexAttribArray(mFaceColAttr);
            glDisableVertexAttribArray(mTexAttr);
            glDisableVertexAttribArray(mNormalAttr);

            mProgram[FACE]->release();
        }

    }

    if (!depthTest) {
        glDisable(GL_DEPTH_TEST);
    }

    //mProgram->release();


}

SceneShaded::~SceneShaded()
{
    delete_safe(mMesh);
}

