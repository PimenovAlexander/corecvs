/**
 * \file scene3D.cpp
 *
 * \date Nov 13, 2012
 **/

#include <QtOpenGL/QtOpenGL>

#include "opengl/openGLTools.h"
#include "scene3D.h"
#include "uis/cloudview/cloudViewDialog.h"
#include "math/matrix/matrix44.h"


Scene3D::~Scene3D()
{
}

void Scene3D::draw(CloudViewDialog *dialog)
{
    if (!visible) {
//        qDebug() << name << "(" << ((void *)this) << ")" << " is invisible";
        return;
    }
//    qDebug() << name << "(" << ((void *)this) << ")" << " is drawn";

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
      OpenGLTools::glMultMatrixMatrix44(transform);
      this->drawMyself(dialog);
    glPopMatrix();
}


void Scene3D::initDecalState(CloudViewDialog *dialog, const Draw3dParameters *parameters, Matrix44 &projector)
{
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_GEN_R);
    glEnable(GL_TEXTURE_GEN_Q);

    qDebug() << "Decal Matrix type " << parameters->decalMatrixType();
    bool eyeLinear =  (parameters->decalMatrixType() % 10) < 5;
    GLint decalMode = eyeLinear ? GL_EYE_LINEAR : GL_OBJECT_LINEAR;
    GLint planeMode = eyeLinear ? GL_EYE_PLANE  : GL_OBJECT_PLANE;

    glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, decalMode);
    glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, decalMode);
    glTexGeni (GL_R, GL_TEXTURE_GEN_MODE, decalMode);
    glTexGeni (GL_Q, GL_TEXTURE_GEN_MODE, decalMode);

    Matrix44 bias = Matrix44(Matrix33::Scale3(0.5), Vector3dd(0.5, 0.5, 0.5));

    /*double zNear = 0.5;
    double zFar  = 100.0;

    double fovyRad = degToRad(45);
    double f = 1.0 / tan(fovyRad / 2);
    double zDepth = zNear - zFar;
    Matrix44 matrixProj (
        f / 1.0   , 0 ,             0          ,            0            ,
        0         ,-f ,             0          ,            0            ,
        0         , 0 , -(zFar + zNear) / zDepth, 2 *zFar * zNear / zDepth,
        0         , 0 ,             1          ,            0
    );*/

    Matrix44 texGen(1.0);


    switch( parameters->decalMatrixType() % 5)
    {
        case 0: texGen = Matrix44(1.0);                           break;
        case 1: texGen = dialog->mCamera.inverted();              break;
        case 2: texGen = dialog->mCamera;                         break;
        case 3: texGen = dialog->mCamera.inverted().transposed(); break;
        case 4: texGen = dialog->mCamera.transposed();            break;
    }

    if (parameters->decalMatrixType() > 10)
    {
        texGen = bias * projector * texGen;
    }

    cout << texGen << endl;

    if (eyeLinear) {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
    }

    glTexGendv(GL_S, planeMode, &texGen.a(0,0));
    glTexGendv(GL_T, planeMode, &texGen.a(1,0));
    glTexGendv(GL_R, planeMode, &texGen.a(2,0));
    glTexGendv(GL_Q, planeMode, &texGen.a(3,0));

    if (eyeLinear) {
        glPopMatrix();
    }
}

void Scene3D::commonSetup(CloudViewDialog *dialog, const Draw3dParameters *parameters)
{
    if (parameters->textureCorrodinates() == Draw3dTextureGen::FORCE_TEXTURE) {
        Matrix44 textureMatrix = Matrix44::Scale(parameters->textureScale());
        glEnable(GL_TEXTURE_GEN_S);
        glEnable(GL_TEXTURE_GEN_T);

        GLint mode = GL_OBJECT_LINEAR;
        glTexGeni (GL_S, GL_TEXTURE_GEN_MODE, mode);
        glTexGendv(GL_S, GL_OBJECT_PLANE, &textureMatrix.a(0,0));

        glTexGeni (GL_T, GL_TEXTURE_GEN_MODE, mode);
        glTexGendv(GL_T, GL_OBJECT_PLANE, &textureMatrix.a(1,0));
    }


    if (parameters->decalRightCam()) {
#ifdef WITH_OPENGLEXT
        CameraIntrinsicsLegacy *intrinsics = &(dialog->mRectificationResult->rightCamera);
        double fovR = intrinsics->getVFov();
        double aspect = intrinsics->resolution.x() / intrinsics->resolution.y();
        Matrix44 projector = Matrix44::Frustum(fovR, aspect);

        glActiveTexture(GL_TEXTURE1);
            glEnable(GL_TEXTURE_2D);
            if (dialog->texture(Frames::RIGHT_FRAME) != (GLuint)-1) {
                glBindTexture(GL_TEXTURE_2D, dialog->texture(Frames::RIGHT_FRAME));
            } else {
                qDebug() << "No texture for right camera";
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
            initDecalState(dialog, parameters, projector);



        glActiveTexture(GL_TEXTURE0);
#else
        CORE_UNUSED(dialog);
#endif
    }

    if (parameters->decalLeftCam()) {
#ifdef WITH_OPENGLEXT
        RectificationResult *rectification = dialog->mRectificationResult.data();
        CameraIntrinsicsLegacy *intrinsics = &(rectification->leftCamera);
        double fovL = intrinsics->getVFov();
        double aspect = intrinsics->resolution.x() / intrinsics->resolution.y();
        Matrix44 projector = Matrix44::Frustum(fovL, aspect);
        projector = projector * (Matrix44(rectification->decomposition.rotation) * Matrix44::Shift(rectification->getCameraShift())).inverted();

        glActiveTexture(GL_TEXTURE2);
            glEnable(GL_TEXTURE_2D);

            if (dialog->texture(Frames::LEFT_FRAME) != (GLuint)-1) {
                glBindTexture(GL_TEXTURE_2D, dialog->texture(Frames::LEFT_FRAME));
            } else {
                qDebug() << "No texture for left camera";
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T    , GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
            initDecalState(dialog, parameters, projector);

        if (parameters->decalRightCam()) {
        	qDebug() << "Both cameras. Making a merge";
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);   //Interpolate RGB with RGB
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
            //GL_CONSTANT refers to the call we make with glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor)
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_COLOR);
            //------------------------
            glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_INTERPOLATE);   //Interpolate ALPHA with ALPHA
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA, GL_PREVIOUS);
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_ALPHA, GL_TEXTURE);
            //GL_CONSTANT refers to the call we make with glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor)
            glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE2_ALPHA, GL_CONSTANT);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
            glTexEnvi(GL_TEXTURE_ENV, GL_OPERAND2_ALPHA, GL_SRC_ALPHA);
            //------------------------
            float mycolor[4] = {0.5, 0.5, 0.5, 0.5};
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, mycolor);

        }
        glActiveTexture(GL_TEXTURE0);
#endif
    }

}

void Scene3D::commonTierdown(CloudViewDialog * /*dialog*/, const Draw3dParameters *parameters)
{
    if (parameters->textureCorrodinates() == Draw3dTextureGen::FORCE_TEXTURE) {
        glDisable(GL_TEXTURE_GEN_S);
        glDisable(GL_TEXTURE_GEN_T);
    }

    if (parameters->decalRightCam()) {
#ifdef WITH_OPENGLEXT
        glActiveTexture(GL_TEXTURE1);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_TEXTURE_GEN_Q);
            glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
#endif
    }

    if (parameters->decalLeftCam()) {
#ifdef WITH_OPENGLEXT
        glActiveTexture(GL_TEXTURE2);
            glDisable(GL_TEXTURE_GEN_S);
            glDisable(GL_TEXTURE_GEN_T);
            glDisable(GL_TEXTURE_GEN_R);
            glDisable(GL_TEXTURE_GEN_Q);
            glDisable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
#endif
    }
}


void Scene3D::prepareMesh(CloudViewDialog * /*dialog*/)
{
    SYNC_PRINT(("Scene3D::prepareMesh(): called"));
}


void Scene3D::drawMyself(CloudViewDialog * /*dialog*/ )
{
    //qDebug() << "Calling Scene3D::Scene3D::drawMyself() for" << name;
}

bool Scene3D::dump(const QString &targetFile)
{
    qDebug("Scene3D::dump(%s):called\n", targetFile.toLatin1().constData());
    return false;
}

