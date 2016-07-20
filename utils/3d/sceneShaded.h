#ifndef SCENESHADED_H
#define SCENESHADED_H

#include <QtOpenGL/QtOpenGL>
#include <QtOpenGL/QGLFunctions>

#include "draw3dCameraParametersControlWidget.h"
#include "scene3D.h"
#include "mesh3DDecorated.h"

class QOpenGLShaderProgram;



class SceneShaded : public Scene3D, public QGLFunctions
{
public:
    Draw3dCameraParameters mParameters;


    QOpenGLShaderProgram *mProgram;
    GLuint mPosAttr;
    GLuint mColAttr;
    GLuint mMatrixUniform;

    /* Some test data */
    Mesh3DDecorated *mesh;


    virtual ParametersControlWidgetBase *getContolWidget() override
    {
        Draw3dCameraParametersControlWidget *result = new Draw3dCameraParametersControlWidget();
        result->setParameters(mParameters);
        return result;
    }

    virtual void setParameters(void * params)  override
    {
        mParameters = *static_cast<Draw3dCameraParameters *>(params);
    }

    virtual void prepareMesh(CloudViewDialog * /*dialog*/) override;
    virtual void drawMyself(CloudViewDialog *dialog)  override;

    virtual ~SceneShaded() {}
};

#endif // SCENESHADED_H
