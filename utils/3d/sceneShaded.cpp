#include <qglfunctions.h>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>
#include <QOpenGLFunctions_4_5_Core>

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
    //SYNC_PRINT(("SceneShaded::setParameters()\n"));
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

        SYNC_PRINT(("SceneShaded::applyParameters(): Creating %d program\n", target));
        mProgram[target] = new QOpenGLShaderProgram();
        mProgram[target]->addShaderFromSourceCode(QOpenGLShader::Vertex,   vShader);
        mProgram[target]->addShaderFromSourceCode(QOpenGLShader::Fragment, fShader);
        mProgram[target]->link();

        if (!mProgram[target]->isLinked())
        {
            qDebug() << mProgram[target]->log();
        }

        SYNC_PRINT(("SceneShaded::applyParameters(): Creating uniform value handles \n"));
        mAmbientUnif  = mProgram[target]->uniformLocation("ambientUnif");
        mDiffuseUnif  = mProgram[target]->uniformLocation("mDiffuseUnif");
        mSpecularUnif = mProgram[target]->uniformLocation("mSpecularUnif");


        SYNC_PRINT(("SceneShaded::applyParameters(): Creating attribute value handles \n"));
        mPosAttr     = mProgram[target]->attributeLocation("posAttr");
        mColAttr     = mProgram[target]->attributeLocation("colAttr");
        mFaceColAttr = mProgram[target]->attributeLocation("faceColAttr");
        mTexAttr     = mProgram[target]->attributeLocation("texAttr");
        mTexIdAttr   = mProgram[target]->attributeLocation("texIdAttr");
        mNormalAttr  = mProgram[target]->attributeLocation("normalAttr");

        mModelViewMatrix  = mProgram[target]->uniformLocation("modelview");
        mProjectionMatrix = mProgram[target]->uniformLocation("projection");

        SYNC_PRINT(("SceneShaded::applyParameters(): Creating samplers value handles \n"));
        mTextureSampler      = mProgram[target]->uniformLocation("textureSampler");
        mMultiTextureSampler = mProgram[target]->uniformLocation("multiTextureSampler");
        mBumpSampler    = mProgram[target]->uniformLocation("bumpSampler");

    }
    mParamsApplied = true;
}

void SceneShaded::prepareTextures(CloudViewDialog * dialog)
{   

    if (mTexturesUpdated) {
        return;
    }
    SYNC_PRINT(("void SceneShaded::prepareTextures():called\n"));

    if (dialog == NULL) {
        SYNC_PRINT(("void SceneShaded::prepareTextures(): Error: dialog is NULL\n"));
        return;
    }

    if (mMesh == NULL) {
        SYNC_PRINT(("void SceneShaded::prepareTextures(): Error: mesh is NULL\n"));
        return;
    }

    SYNC_PRINT(("void SceneShaded::prepareTextures(): Mesh has %d matrials\n", (int)mMesh->materials.size()));

    dialog->mUi.widget->makeCurrent();
    QOpenGLFunctions_4_5_Core &glFuncs = *(dialog->mUi.widget->context()->versionFunctions<QOpenGLFunctions_4_5_Core>());


    mTextures.resize(mMesh->materials.size(), -1);
    mBumpmaps.resize(mMesh->materials.size(), -1);

    for (size_t materialId = 0; materialId < mMesh->materials.size(); materialId++)
    {
        SYNC_PRINT(("void SceneShaded::prepareTextures(): Processing material %d (%d)\n", (int)materialId, (int)mMesh->materials.size()));


        /*Prepare Texture*/
        RGB24Buffer *texBuf = mMesh->materials.size() > 0 ? mMesh->materials.front().tex[OBJMaterial::TEX_DIFFUSE] : NULL;
        if (texBuf != NULL) {
            glFuncs.glEnable(GL_TEXTURE_2D);
            qDebug() << "Dumping prior error";
            dumpGLErrors();
            glFuncs.glGenTextures(1, &mTextures[materialId]);
            qDebug() << "Created a handle for the texture:" << mTextures[materialId];
            dumpGLErrors();
            addTexture(mTextures[materialId], texBuf);
            glFuncs.glDisable(GL_TEXTURE_2D);
        } else {
            SYNC_PRINT(("void SceneShaded::prepareTextures(): Message: texBuf for material %d is NULL\n", (int)materialId));
        }

        /*Prepare Bumpmap*/
        RGB24Buffer *bumpBuf = mMesh->materials.size() > 0 ? mMesh->materials.front().tex[OBJMaterial::TEX_BUMP] : NULL;
        if (bumpBuf != NULL) {
            glFuncs.glEnable(GL_TEXTURE_2D);
            qDebug() << "Dumping prior error";
            dumpGLErrors();
            glFuncs.glGenTextures(1, &mBumpmaps[materialId]);
            qDebug() << "Created a handle for the bumpmap:" << mBumpmaps[materialId];
            dumpGLErrors();
            addTexture(mBumpmaps[materialId], bumpBuf);
            glFuncs.glDisable(GL_TEXTURE_2D);
        } else {
            SYNC_PRINT(("void SceneShaded::prepareTextures(): Message: bumpBuf for material %d is NULL\n", (int)materialId));
        }
    }


#if 0
    if (!mMesh->materials.empty())
    {
        RGB24Buffer *texBuf = mMesh->materials.front().tex[OBJMaterial::TEX_DIFFUSE];
        if (texBuf != NULL)
        {
            glFuncs.glGenTextures(1, &mTexArray);
            glFuncs.glBindTexture(GL_TEXTURE_2D_ARRAY,mTexArray);

            GLint oldStride;
            glFuncs.glGetIntegerv(GL_UNPACK_ROW_LENGTH, &oldStride);
            glFuncs.glPixelStorei(GL_UNPACK_ROW_LENGTH, texBuf->stride);
            glFuncs.glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, texBuf->w, texBuf->h, mMesh->materials.size());

            for (size_t i = 0; i < mMesh->materials.size(); i++)
            {
                glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, texBuf->w, texBuf->h, 1, GL_RGBA, GL_UNSIGNED_BYTE, &mMesh->materials[i].tex[OBJMaterial::TEX_DIFFUSE]->element(0,0));
            }

            glFuncs.glPixelStorei(GL_UNPACK_ROW_LENGTH, oldStride);


            glFuncs.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glFuncs.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glFuncs.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glFuncs.glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

    }
#endif

    mTexturesUpdated = true;
}


void SceneShaded::prepareMesh(CloudViewDialog * dialog)
{
    if (mMeshPrepared || mMesh == NULL)
    {
        return;
    }
    //SYNC_PRINT(("void SceneShaded::prepareMesh():called\n"));

    /* Prepare caches in OpenGL formats */
    clearCache();
    cache(mMesh, mParameters);

    mMeshPrepared = true;
}

void SceneShaded::drawMyself(CloudViewDialog * dialog)
{
    prepareMesh(dialog);
    prepareTextures(dialog);
    applyParameters();

    dialog->mUi.widget->makeCurrent();

    //QOpenGLFunctions &glFuncs = *(dialog->mUi.widget->context()->functions());
    QOpenGLFunctions_4_5_Core &glFuncs = *(dialog->mUi.widget->context()->versionFunctions<QOpenGLFunctions_4_5_Core>());

    applyParameters();
/*    if (mProgram[] == NULL)
    {
        qDebug("SceneShaded::drawMyself(): mProgram is NULL");
        return;
    }
    mProgram->bind();*/


    SYNC_PRINT(("SceneShaded::drawMyself(): cache state\n"));
    cout << *static_cast<SceneShadedOpenGLCache *>(this) << endl;


    SYNC_PRINT(("SceneShaded::drawMyself(): Preparing matrices\n"));
    float arr[16];
    glFuncs.glGetFloatv(GL_MODELVIEW_MATRIX, arr);
    QMatrix4x4 modelview(
            arr[0], arr[4], arr[ 8], arr[12],
            arr[1], arr[5], arr[ 9], arr[13],
            arr[2], arr[6], arr[10], arr[14],
            arr[3], arr[7], arr[11], arr[15]
    );

    glFuncs.glGetFloatv(GL_PROJECTION_MATRIX, arr);
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

    bool depthTest =  glFuncs.glIsEnabled(GL_DEPTH_TEST);
    glFuncs.glEnable(GL_DEPTH_TEST);

    // qDebug() << "Before Mesh Draw";   dumpGLErrors();

    /* Draw embedded Mesh */
    if (mMesh != NULL)
    {
        /* Draw points */
        SYNC_PRINT(("SceneShaded::drawMyself(): Draw points\n"));

        if (mParameters.point.type != ShaderPreset::NONE && mProgram[POINT] != NULL)
        {
            mProgram[POINT]->bind();
            int oldPointSize = 1;

            glFuncs.glGetIntegerv(GL_POINT_SIZE, &oldPointSize);
            glPointSize(mParameters.pointSize());

            // cout << "Executing point draw" << endl;
            glFuncs.glVertexAttribPointer(mPosAttr, 3, GL_FLOAT, GL_FALSE, 0, positions.data());
            glFuncs.glEnableVertexAttribArray(mPosAttr);

            if (mMesh->hasColor && !mParameters.pointColorOverride())
            {
                glFuncs.glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), mMesh->vertexesColor.data());
                glFuncs.glEnableVertexAttribArray(mColAttr);
            } else {
                Vector3df pointColor = RGBColor(mParameters.pointColor()).toFloat();
                glFuncs.glVertexAttrib3f(mColAttr, pointColor.x(), pointColor.y(), pointColor.z());
            }

            //SYNC_PRINT(("Calling glDrawArrays(%d %d %d)\n", GL_POINTS, 0, static_cast<GLsizei>(mMesh->vertexes.size()));
            glFuncs.glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mMesh->vertexes.size()));

            glFuncs.glDisableVertexAttribArray(mPosAttr);
            glFuncs.glDisableVertexAttribArray(mColAttr);

            glPointSize(oldPointSize);
            mProgram[POINT]->release();
        }

        SYNC_PRINT(("SceneShaded::drawMyself(): Draw edges\n"));

        if (mParameters.edge.type != ShaderPreset::NONE && mProgram[EDGE] != NULL)
        {
            mProgram[EDGE]->bind();
            int oldLineWidth = 1;
            glFuncs.glGetIntegerv(GL_LINE_WIDTH, &oldLineWidth);
            glFuncs.glLineWidth(mParameters.edgeWidth());

            glFuncs.glVertexAttribPointer(mPosAttr, 3, GL_FLOAT, GL_FALSE, 0, edgePositions.data());
            glFuncs.glEnableVertexAttribArray(mPosAttr);

            if (!edgeColors.empty())
            {
                glFuncs.glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), edgeColors.data());
                glFuncs.glEnableVertexAttribArray(mColAttr);
            }

            glFuncs.glDrawElements(GL_LINES, GLsizei(edgeIds.size()), GL_UNSIGNED_INT, edgeIds.data());

            glFuncs.glDisableVertexAttribArray(mPosAttr);
            glFuncs.glDisableVertexAttribArray(mColAttr);

            glFuncs.glLineWidth(oldLineWidth);
            mProgram[EDGE]->release();
        }

        /* Draw faces */        
        SYNC_PRINT(("SceneShaded::drawMyself(): Draw faces\n"));

        if (mParameters.face.type != ShaderPreset::NONE && mProgram[FACE] != NULL)
        {
            mProgram[FACE]->bind();

            for (size_t materialId = 0; materialId < mMesh->materials.size(); materialId++)
            {
                SYNC_PRINT(("SceneShaded::drawMyself(): Draw faces for material %d\n", materialId));

                {
                    Vector3dd color = mMesh->materials[materialId].koefs[OBJMaterial::KOEF_AMBIENT];
                    QVector3D colorAmb(color.x(), color.y(), color.z());
                    mProgram[FACE]->setUniformValue(mAmbientUnif , colorAmb);
                }
                {
                    Vector3dd color = mMesh->materials[materialId].koefs[OBJMaterial::KOEF_DIFFUSE];
                    QVector3D colorDiff(color.x(), color.y(), color.z());
                    mProgram[FACE]->setUniformValue(mDiffuseUnif , colorDiff);
                }
                {
                    Vector3dd color = mMesh->materials[materialId].koefs[OBJMaterial::KOEF_SPECULAR];
                    QVector3D colorSpec(color.x(), color.y(), color.z());
                    mProgram[FACE]->setUniformValue(mSpecularUnif , colorSpec);
                }

                SYNC_PRINT(("SceneShaded::drawMyself(): Uniform variables set\n"));

                SceneShadedFaceCache &faces = faceCache[materialId];

                glFuncs.glVertexAttribPointer(mPosAttr, 3, GL_FLOAT, GL_FALSE, 0, faces.facePositions.data());
                glFuncs.glEnableVertexAttribArray(mPosAttr);

                SYNC_PRINT(("SceneShaded::drawMyself(): Face position set\n"));

                if (!faces.faceColors.empty() && !faces.faceVertexColors.empty())
                {
                    glFuncs.glVertexAttribPointer(mColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), faces.faceColors.data());
                    glFuncs.glEnableVertexAttribArray(mColAttr);

                    glFuncs.glVertexAttribPointer(mFaceColAttr, GL_BGRA, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(RGBColor), faces.faceVertexColors.data());
                    glFuncs.glEnableVertexAttribArray(mFaceColAttr);

                }
                SYNC_PRINT(("SceneShaded::drawMyself(): Face color set\n"));

                if (mMesh->hasTexCoords && !faces.faceTexCoords.empty())
                {
                    glFuncs.glVertexAttribPointer(mTexAttr, 2, GL_FLOAT, GL_FALSE, 0, faces.faceTexCoords.data());
                    glFuncs.glEnableVertexAttribArray(mTexAttr);

                    glFuncs.glVertexAttribIPointer(mTexIdAttr, 1, GL_UNSIGNED_INT, 0, faces.faceTexNums.data());

                    glFuncs.glEnableVertexAttribArray(mTexIdAttr);
                }
                SYNC_PRINT(("SceneShaded::drawMyself(): Face tex coords set\n"));

                if (mMesh->hasNormals && !faces.faceNormals.empty())
                {
                    glFuncs.glVertexAttribPointer(mNormalAttr, 3, GL_FLOAT, GL_FALSE, 0, faces.faceNormals.data());
                    glFuncs.glEnableVertexAttribArray(mNormalAttr);
                }
                SYNC_PRINT(("SceneShaded::drawMyself(): Face normals set\n"));

                GLboolean oldTexEnable = glIsEnabled(GL_TEXTURE_2D);


                SYNC_PRINT(("SceneShaded::drawMyself(): Binding textures\n"));

                if (mTextures[materialId] != (GLuint)(-1))
                {
                    //qDebug() << "Before Texture";   dumpGLErrors();
                    glEnable(GL_TEXTURE_2D);
                    glActiveTexture(GL_TEXTURE0);
                    //qDebug() << "Before Bind";   dumpGLErrors();
                    glBindTexture(GL_TEXTURE_2D, mTextures[materialId]);
                    //qDebug() << "Before Sampler";   dumpGLErrors();
                    mProgram[FACE]->setUniformValue(mTextureSampler, 0);
                    //qDebug() << "Before Call"; dumpGLErrors();
                } else {
                    SYNC_PRINT(("SceneShaded::drawMyself(): no texture for material\n"));
                }

#if 0
                /* Multitexture */
                glEnable(GL_TEXTURE_2D_ARRAY);
                mProgram[FACE]->setUniformValue(mMultiTextureSampler, 0);
#endif

                SYNC_PRINT(("SceneShaded::drawMyself(): Binding bumpmaps \n"));

                if (mBumpmaps[materialId] != (GLuint)(-1))
                {
                    glEnable(GL_TEXTURE_2D);
                    glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, mBumpmaps[materialId]);
                    mProgram[FACE]->setUniformValue(mBumpSampler, 1);
                } else {
                    SYNC_PRINT(("SceneShaded::drawMyself(): no bumpmap material\n"));
                }

                SYNC_PRINT(("SceneShaded::drawMyself(): Requesting draw\n"));
                glDrawElements(GL_TRIANGLES, GLsizei(faces.faceIds.size()), GL_UNSIGNED_INT, faces.faceIds.data());

                SYNC_PRINT(("SceneShaded::drawMyself(): Unbinding\n"));
                glBindTexture(GL_TEXTURE_2D, 0);
                if (!oldTexEnable) {
                    glActiveTexture(GL_TEXTURE0);
                    glDisable(GL_TEXTURE_2D);

                    glActiveTexture(GL_TEXTURE1);
                    glDisable(GL_TEXTURE_2D);
                }

                SYNC_PRINT(("SceneShaded::drawMyself(): cleanup \n"));
                glFuncs.glDisableVertexAttribArray(mPosAttr);
                glFuncs.glDisableVertexAttribArray(mColAttr);
                glFuncs.glDisableVertexAttribArray(mFaceColAttr);
                glFuncs.glDisableVertexAttribArray(mTexAttr);
                glFuncs.glDisableVertexAttribArray(mTexIdAttr);
                glFuncs.glDisableVertexAttribArray(mNormalAttr);

            } // for over materials

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

