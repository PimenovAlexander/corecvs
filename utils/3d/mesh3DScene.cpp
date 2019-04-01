/**
 * \file mesh3D.cpp
 *
 * \date Nov 13, 2012
 **/

#include <QtCore/QDebug>

#include <QOpenGLFunctions_4_5_Core>

#include "core/utils/global.h"

#include "core/math/mathUtils.h"
#include "core/fileformats/meshLoader.h"
#include "mesh3DScene.h"
#include "opengl/openGLTools.h"
#include "generated/draw3dParameters.h"
#include "painterHelpers.h"
#include "core/math/mathUtils.h"
#include "qtHelper.h"

using corecvs::lerp;

void Mesh3DScene::drawMyself(CloudViewDialog *dialog)
{
    //SYNC_PRINT(("Mesh3DScene::drawMyself() : called\n" ));
    if (owned == NULL) {
        return;
    }

    bool withTexture = false;
    withTexture |= (mParameters.style() == Draw3dStyle::TEXTURED) && (dialog->mFancyTexture != GLuint(-1));
    //bool withTexCoords = withTexture && !textureCoords.empty();
    bool withTexCoords = false;

    /*Caption drawing*/
    if (mParameters.showCaption())
    {
        if (owned->hasHook)
        {
            OpenGLTools::GLWrapper wrapper;
            GLPainter painter(&wrapper);

            QString text = QString("[%1, %2, %3]")
                    .arg(owned->hookPoint.x(), 0, 'f', 2)
                    .arg(owned->hookPoint.y(), 0, 'f', 2)
                    .arg(owned->hookPoint.z(), 0, 'f', 2);

            Matrix44 modelview  = OpenGLTools::glGetModelViewMatrix();
            Matrix44 projection = OpenGLTools::glGetProjectionMatrix();
            Matrix44 glMatrix = projection * modelview;

            /* Setting new matrices */
            int width  = dialog->mUi.widget->width();
            int height = dialog->mUi.widget->height();

            /* SetMatrices to draw on 2D canvas */
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(- width / 2.0 , width / 2.0, -height / 2.0, height / 2.0, 10, -10);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

            Vector3dd labelPos = glMatrix * owned->hookPoint;
            glTranslated(labelPos[0] * width / 2.0, labelPos[1] * height / 2.0, 0.0);

            double size = mParameters.fontSize() / 25.0;
            glScaled(size, -size, size);

            bool depthTest =  glIsEnabled(GL_DEPTH_TEST);
            glDisable(GL_DEPTH_TEST);
            GLint lineWidth;
            glGetIntegerv(GL_LINE_WIDTH, &lineWidth);
            glLineWidth(mParameters.fontWidth());

            //glColor3ub(mParameters.fontColor().r(), mParameters.fontColor().g(), mParameters.fontColor().b());

            painter.drawFormatVector(0, 0, mParameters.fontColor(), 1, text.toLatin1().constData());
            if (depthTest) {
                glEnable(GL_DEPTH_TEST);
            }
            glLineWidth(lineWidth);

            /*Restore old matrices */
            glMatrixMode(GL_PROJECTION);
            glPopMatrix();
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();


        }
    }

    if (!withTexture) {
        glDisable(GL_TEXTURE_2D);
    } else {
        glEnable(GL_TEXTURE_2D);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glBindTexture(GL_TEXTURE_2D, dialog->mFancyTexture);
    }

    commonSetup(dialog, &mParameters);

    GLenum mode;
    switch (mParameters.style()) {
        case Draw3dStyle::POINTS:
            mode = GL_POINT;
            break;
        case Draw3dStyle::WIREFRAME:
            mode = GL_LINE;
            break;
        default:
        case Draw3dStyle::COLOR:
        case Draw3dStyle::COLOR_2:
            mode = GL_FILL;
            break;
    }

    glEnableClientState(GL_VERTEX_ARRAY);

    if (withTexCoords) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (mParameters.style() == Draw3dStyle::COLOR_2 && owned->faces.size() > 0)
    {
        glPolygonOffset(-1.0, -2.0);
        glEnable       (GL_POLYGON_OFFSET_LINE);
        glPolygonMode  (GL_FRONT_AND_BACK, GL_LINE);
        glColor3ub     (mParameters.edgeColor().r(), mParameters.edgeColor().g(), mParameters.edgeColor().b());
        glVertexPointer(3, GL_DOUBLE, sizeof(Vector3dd), &(owned->vertexes[0]));
        glDrawElements (GL_TRIANGLES, GLsizei(owned->faces.size() * 3), GL_UNSIGNED_INT, &(owned->faces[0]));

//        glDrawElements(GL_LINES    , GLsizei(edges.size() * 2), GL_UNSIGNED_INT, &(edges[0]));
        glDisable      (GL_POLYGON_OFFSET_LINE);
    }

    glPolygonMode(GL_FRONT_AND_BACK, mode);

   // glColor3ub(mParameters.color().r(), mParameters.color().g(), mParameters.color().b());
    if (owned->vertexes.size() > 0)
    {
        glVertexPointer(3, GL_DOUBLE, sizeof(Vector3dd), &(owned->vertexes[0]));
    }

/*
    if (withTexCoords && textureCoords.size() > 0)
    {
        glTexCoordPointer(2, GL_DOUBLE, sizeof(Vector2dd), &(textureCoords[0]));
    }
*/

    /* FACES */
    if (owned->faces.size() > 0)
    {
        if (mParameters.faceColorOverride() || !owned->hasColor) {
            OpenGLTools::glColorRGB(mParameters.faceColor());
            glDrawElements(GL_TRIANGLES, GLsizei(owned->faces.size() * 3), GL_UNSIGNED_INT, &(owned->faces[0]));
        }
        else {
            /* We need to speed this up */
            for (size_t fi = 0; fi < owned->faces.size(); fi++)
            {
                OpenGLTools::glColorRGB(owned->facesColor[fi]);
                glDrawElements(GL_TRIANGLES, GLsizei(3), GL_UNSIGNED_INT, &(owned->faces[fi]));
            }
        }
    }

    /* EDGES */
    int oldLineWidth = 1;
    glGetIntegerv(GL_LINE_WIDTH, &oldLineWidth);
    glLineWidth(mParameters.edgeWidth());

    if (mParameters.edgeColorOverride() || !owned->hasColor ) {
        OpenGLTools::glColorRGB(mParameters.edgeColor());
        glDrawElements(GL_LINES, GLsizei(owned->edges.size() * 2), GL_UNSIGNED_INT, &(owned->edges[0]));
    }
    else {
        for (size_t ei = 0; ei < owned->edges.size(); ei++)
        {
            OpenGLTools::glColorRGB(owned->edgesColor[ei]);
            glDrawElements(GL_LINES, GLsizei(2), GL_UNSIGNED_INT, &(owned->edges[ei]));
        }
    }
    glPointSize(oldLineWidth);


    /* POINTS */
    int oldPointSize = 1;
    glGetIntegerv(GL_POINT_SIZE, &oldPointSize);
    glPointSize(mParameters.pointSize());

    if (mParameters.pointColorOverride() || !owned->hasColor ) {
        glColor3ub(mParameters.pointColor().r(), mParameters.pointColor().g(), mParameters.pointColor().b());

        glDrawArrays(GL_POINTS, 0, (int)owned->vertexes.size());
    }
    else {
        glColor3ub(mParameters.pointColor().r(), mParameters.pointColor().g(), mParameters.pointColor().b());
        glPointSize(mParameters.pointSize());
        /*glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_UNSIGNED_INT, sizeof(RGBColor), &(vertexesColor[0]));
        glDrawArrays (GL_POINTS, 0, vertexes.size());
        glDisableClientState(GL_COLOR_ARRAY);*/

        glBegin(GL_POINTS); /* This is slow and retarded*/
        for (size_t vi = 0; vi < owned->vertexes.size(); vi++)
        {
            OpenGLTools::glColorRGB(owned->vertexesColor[vi]);
            glVertex3dv((double *)&(owned->vertexes[vi]));
        }
        glEnd();
    }
    glPointSize(oldPointSize);

    /* Cleanup */
    glDisableClientState(GL_VERTEX_ARRAY);
    if (withTexCoords) {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    commonTierdown(dialog, &mParameters);

    glDisable(GL_TEXTURE_2D);
}

bool Mesh3DScene::dump(const QString &targetFile)
{
   if (owned != NULL)
   {
       qDebug("Mesh3DScene::dump(): saving owned scene\n");
       return MeshLoader().save(owned, targetFile.toLatin1().constData());
   } else {
       qDebug("Mesh3DScene::dump(): there is no owned scene\n");
       return false;
   }
}




void CameraScene::prepareMesh(CloudViewDialog * /*dialog*/)
{}

void CameraScene::drawMyself(CloudViewDialog * /*dialog*/ )
{
    //qDebug() << "Calling CameraScene::drawMyself() for" << name;

    glPushMatrix();
      glColor3ub(mParameters.edgeColor().r(), mParameters.edgeColor().g(), mParameters.edgeColor().b());
      glScaled(5.0,5.0,5.0);
      OpenGLTools::drawWiredCamera();
    glPopMatrix();


    glColor3ub(255, 0, 255);

    double nearPlane = mParameters.nearPlane();
    double farPlane  = mParameters.farPlane();
    double alphaH = degToRad(mParameters.fovH()) / 2;
    double alphaV = degToRad(mParameters.fovV()) / 2;

    double x1, y1;
    x1 = nearPlane * tan (alphaH);
    y1 = nearPlane * tan (alphaV);

    double x2, y2;
    x2 = farPlane  * tan (alphaH);
    y2 = farPlane  * tan (alphaV);


    glBegin(GL_LINES);
    /* One side */
        glVertex3d(-x1, -y1, nearPlane);
        glVertex3d( x1, -y1, nearPlane);

        glVertex3d( x1, -y1, nearPlane);
        glVertex3d( x1,  y1, nearPlane);

        glVertex3d( x1,  y1, nearPlane);
        glVertex3d(-x1,  y1, nearPlane);

        glVertex3d(-x1,  y1, nearPlane);
        glVertex3d(-x1, -y1, nearPlane);

    /* Other side */
        glVertex3d(-x2, -y2, farPlane);
        glVertex3d( x2, -y2, farPlane);

        glVertex3d( x2, -y2, farPlane);
        glVertex3d( x2,  y2, farPlane);

        glVertex3d( x2,  y2, farPlane);
        glVertex3d(-x2,  y2, farPlane);

        glVertex3d(-x2,  y2, farPlane);
        glVertex3d(-x2, -y2, farPlane);

    /*Connections*/
        glVertex3d( x1,  y1, nearPlane);
        glVertex3d( x2,  y2, farPlane);

        glVertex3d(-x1,  y1, nearPlane);
        glVertex3d(-x2,  y2, farPlane);

        glVertex3d( x1, -y1, nearPlane);
        glVertex3d( x2, -y2, farPlane);

        glVertex3d(-x1, -y1, nearPlane);
        glVertex3d(-x2, -y2, farPlane);
    glEnd();

}

StereoCameraScene::StereoCameraScene(const RectificationResult &result)
    : mStereoPair(result)
{}

void StereoCameraScene::prepareMesh(CloudViewDialog *dialog)
{
    mCamera1.prepareMesh(dialog);
    mCamera2.prepareMesh(dialog);
}

void StereoCameraScene::drawMyself(CloudViewDialog *dialog)
{
    glMatrixMode(GL_MODELVIEW);
    Draw3dCameraParameters camParams;
    camParams.setEdgeColor(RgbColorParameters(255,20,20));
    camParams.setFovH(radToDeg(mStereoPair.rightCamera.getHFov()));
    camParams.setFovV(radToDeg(mStereoPair.rightCamera.getVFov()));
    mCamera1.setParameters(&camParams);
    mCamera1.drawMyself(dialog);
    OpenGLTools::GLWrapper wrapper;
    GLPainter painter(&wrapper);
    glPushMatrix();
    glTranslated(-20, -30, 0);
    glScaled(0.5,0.5,0.5);
    painter.drawFormatVector(0, 0, RGBColor(255,20,20), 1, "Right");
    glPopMatrix();

    glPushMatrix();
    OpenGLTools::glTranslateVector3dd(mStereoPair.getCameraShift());
    OpenGLTools::glMultMatrixMatrix33(mStereoPair.decomposition.rotation);
    camParams.setEdgeColor(RgbColorParameters(20,20,200));
    camParams.setFovH(radToDeg(mStereoPair.leftCamera.getHFov()));
    camParams.setFovV(radToDeg(mStereoPair.leftCamera.getVFov()));
    mCamera2.setParameters(&camParams);
    mCamera2.drawMyself(dialog);
    glTranslated(-20, -30, 0);
    glScaled(0.5,0.5,0.5);
    painter.drawFormatVector(0, 0, RGBColor(20,20,200), 1, "Left");
    glPopMatrix();
}

