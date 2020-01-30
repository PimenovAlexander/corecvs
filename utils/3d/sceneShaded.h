#ifndef SCENESHADED_H
#define SCENESHADED_H

#include <QtOpenGL/QtOpenGL>
//#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_4_Core>

#include "draw3dCameraParametersControlWidget.h"
#include "scene3D.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/geometry/mesh/meshCache.h"
#include "shadedSceneControlWidget.h"

class QOpenGLShaderProgram;


class SceneShaded : public Scene3D, public SceneShadedOpenGLCache
{
public:
    ShadedSceneControlParameters mParameters;

    QString pointShaderCache;
    QString edgeShaderCache;
    QString faceShaderCache;

    enum ShaderTarget{
        FACE,
        POINT,
        EDGE,
        LAST
    };
    QOpenGLShaderProgram *mProgram[ShaderTarget::LAST];

    GLuint mPosAttr;
    GLuint mColAttr;

    GLuint mFaceColAttr;
    GLuint mTexAttr;
    GLuint mTexIdAttr;
    GLuint mNormalAttr;

    GLuint mTextureSampler;
    GLuint mMultiTextureSampler;
    GLuint mBumpSampler;


    GLuint mModelViewMatrix;
    GLuint mProjectionMatrix;

    /*Textures*/
    GLuint mTexture  = -1;
    GLuint mTexArray = -1;

    GLuint mBumpmap = -1;


protected:
    /** Geometry requires to be repacked for OpenGL to work fast. This flag stores if we need to update geometry cache **/
    bool mMeshPrepared = false;

    /** Are textures uploaded to the video memory **/
    bool mTexturesUpdated = false;

    /**
     * We can recive new paramaters earlier then OpenGL context is created.
     * For example - if widget is created, but not shown - there is yet no context.
     * So we should remember that we have got some parameters to apply, shaders to compile etc...
     **/
    bool mParamsApplied = false;

    /* Main mesh data */
    Mesh3DDecorated *mMesh = NULL;

public:




    SceneShaded()
    {       
        for (int target = 0; target < ShaderTarget::LAST; target++)
        {
            mProgram[target] = NULL;
        }
    }

    virtual ParametersControlWidgetBase *getContolWidget() override
    {
        ShadedSceneControlWidget *result = new ShadedSceneControlWidget();
        result->setParameters(mParameters);
        return result;
    }

    /** Takes ownership **/
    void setMesh(Mesh3DDecorated *newMesh = NULL, bool updateMaterials = false)
    {
        delete_safe(mMesh);
        mMesh = newMesh;
        mMeshPrepared = false;
        if (updateMaterials) {
            mTexturesUpdated = false;
        }
    }

    virtual void setParameters(void * params)  override;

    void applyParameters();

    virtual void prepareTextures(CloudViewDialog *dialog);
    virtual void prepareMesh(CloudViewDialog *dialog) override;

    virtual void drawMyself (CloudViewDialog *dialog)  override;

    virtual ~SceneShaded();
    void addTexture(GLuint texId, RGB24Buffer *input);
};



#endif // SCENESHADED_H
