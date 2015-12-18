/**
 * \file mesh3D.cpp
 *
 * \date Nov 13, 2012
 **/

#include <QtCore/QDebug>

#include "global.h"

#include "mathUtils.h"
#include "mesh3DScene.h"
#include "opengl/openGLTools.h"
#include "generated/draw3dParameters.h"
#include "painterHelpers.h"
#include "mathUtils.h"
#include "qtHelper.h"

using corecvs::lerp;

void Mesh3DScene::drawMyself(CloudViewDialog *dialog)
{
    //qDebug("Mesh3DScene::drawMyself() : called" );
    //qDebug("V: %d E: %d F: %d ", );


    bool withTexture = false;
    withTexture |= (mParameters.style() == Draw3dStyle::TEXTURED) && (dialog->mFancyTexture != GLuint(-1));
    bool withTexCoords = withTexture && !textureCoords.empty();

    /*Caption drawing*/
    if (mParameters.showCaption())
    {
        if (hasCentral)
        {
            OpenGLTools::GLWrapper wrapper;
            GLPainter painter(&wrapper);

            QString text = QString("[%1, %2, %3]")
                    .arg(centralPoint.x(), 0, 'f', 2).arg(centralPoint.y(), 0, 'f', 2).arg(centralPoint.z(), 0, 'f', 2);

            /*glPushMatrix();
               glTranslated(centralPoint.x(),centralPoint.y(), centralPoint.z());
               glScaled(0.5,0.5,0.5);
               painter.drawFormatVector(0, 0, RGBColor(255,20,20), 1, text.toLatin1().constData());
            glPopMatrix();*/

            //Vector3dd projected =

            Matrix44 modelview  = OpenGLTools::glGetModelViewMatrix();
            Matrix44 projection = OpenGLTools::glGetProjectionMatrix();
            Matrix44 glMatrix = projection * modelview;

            /* Setting new matrices */

            int width  = dialog->mUi.widget->width();
            int height = dialog->mUi.widget->height();
            /*double aspect = (double)width / (double)height;
            */
            /*int width  = 200;
            int height = 200;*/

            /* SetMatrices to draw on 2D canvas */
            glMatrixMode(GL_PROJECTION);
            glPushMatrix();
            glLoadIdentity();
            glOrtho(- width / 2.0 , width / 2.0, -height / 2.0, height / 2.0, 10, -10);

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            glLoadIdentity();

/*          glBegin(GL_LINE_LOOP);
              glVertex3i(0,0,0);
              glVertex3i(100,0,0);
              glVertex3i(100,100,0);
              glVertex3i(0,100,0);
            glEnd();*/

            /* Draw */
/*            FixedVector<double, 4> cent4((const FixedVector<double, 3> &)centralPoint, 1.0);
            FixedVector<double, 4> labelPos = glMatrix * cent4;
            glTranslated(labelPos[0] / labelPos[3] * width / 2, labelPos[1] / labelPos[3] * height / 2, 0.0);
*/
            Vector3dd labelPos = glMatrix * centralPoint;
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

    if (mParameters.style() == Draw3dStyle::COLOR_2 && faces.size() > 0)
    {
        glPolygonOffset ( -1.0, -2.0);
        glEnable (  GL_POLYGON_OFFSET_LINE );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        glColor3ub(mParameters.edgeColor().r(), mParameters.edgeColor().g(), mParameters.edgeColor().b());
        glVertexPointer(3, GL_DOUBLE, sizeof(Vector3dd), &(vertexes[0]));
        glDrawElements(GL_TRIANGLES, GLsizei(faces.size() * 3), GL_UNSIGNED_INT, &(faces[0]));

//        glDrawElements(GL_LINES    , GLsizei(edges.size() * 2), GL_UNSIGNED_INT, &(edges[0]));

        glDisable (GL_POLYGON_OFFSET_LINE);
    }

    glPolygonMode( GL_FRONT_AND_BACK, mode );

   // glColor3ub(mParameters.color().r(), mParameters.color().g(), mParameters.color().b());
    glVertexPointer(3, GL_DOUBLE, sizeof(Vector3dd), &(vertexes[0]));
    if (withTexCoords) {
        glTexCoordPointer(2, GL_DOUBLE, sizeof(Vector2dd), &(textureCoords[0]));
    }

    /* FACES */
    if (faces.size() > 0)
    {
        if (mParameters.faceColorOverride() || !hasColor) {
            OpenGLTools::glColorRGB(mParameters.faceColor());
            glDrawElements(GL_TRIANGLES, GLsizei(faces.size() * 3), GL_UNSIGNED_INT, &(faces[0]));
        }
        else {
            /* We need to speed this up */
            for (size_t fi = 0; fi < faces.size(); fi++)
            {
                OpenGLTools::glColorRGB(facesColor[fi]);
                glDrawElements(GL_TRIANGLES, GLsizei(3), GL_UNSIGNED_INT, &(faces[fi]));
            }
        }
    }

    /* EDGES */
    int oldLineWidth = 1;
    glGetIntegerv(GL_LINE_WIDTH, &oldLineWidth);
    glLineWidth(mParameters.edgeWidth());

    if (mParameters.edgeColorOverride() || !hasColor ) {
        OpenGLTools::glColorRGB(mParameters.edgeColor());
        glDrawElements(GL_LINES    , GLsizei(edges.size() * 2), GL_UNSIGNED_INT, &(edges   [0]));
    } else {
        for (size_t ei = 0; ei < edges.size(); ei++)
        {
            OpenGLTools::glColorRGB(edgesColor[ei]);
            glDrawElements(GL_LINES    , GLsizei(2), GL_UNSIGNED_INT, &(edges[ei]));
        }
    }
    glPointSize(oldLineWidth);


    /* POINTS */
    int oldPointSize = 1;
    glGetIntegerv(GL_POINT_SIZE, &oldPointSize);
    glPointSize(mParameters.pointSize());

    if (mParameters.pointColorOverride() || !hasColor ) {
        glColor3ub(mParameters.pointColor().r(), mParameters.pointColor().g(), mParameters.pointColor().b());

        glDrawArrays (GL_POINTS, 0, vertexes.size());
    } else {
        glColor3ub(mParameters.pointColor().r(), mParameters.pointColor().g(), mParameters.pointColor().b());
        glPointSize(mParameters.pointSize());
        /*glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_UNSIGNED_INT, sizeof(RGBColor), &(vertexesColor[0]));
        glDrawArrays (GL_POINTS, 0, vertexes.size());
        glDisableClientState(GL_COLOR_ARRAY);*/

        glBegin(GL_POINTS); /* This is slow and retarded*/
        for (size_t vi = 0; vi < vertexes.size(); vi++)
        {
            OpenGLTools::glColorRGB(vertexesColor[vi]);
            glVertex3dv((double *)&(vertexes[vi]));
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


Mesh3DScene::~Mesh3DScene() {
    // TODO Auto-generated destructor stub
}


const int Grid3DScene::GRID_SIZE = 5;
const int Grid3DScene::GRID_STEP = 250;


void Grid3DScene::prepareMesh(CloudViewDialog* /*dialog*/) {
    //qDebug() << "Calling Grid3DScene::prepareMesh() for" << name;

    mGridId = glGenLists(1);
    if (mGridId == 0) {
        qDebug("Grid3DScene failed to create display list");
        return;
    }

    glNewList(mGridId,GL_COMPILE);

    glPointSize(2);
    glBegin(GL_POINTS);

    for (int i = -GRID_SIZE; i <= GRID_SIZE; i++)
        for (int j = -GRID_SIZE; j <= GRID_SIZE; j++)
            for (int k = -GRID_SIZE; k <= GRID_SIZE; k++)
            {
                glColor4f(
                    (GLfloat)(i + GRID_SIZE) / (2 * GRID_SIZE + 1),
                    (GLfloat)(j + GRID_SIZE) / (2 * GRID_SIZE + 1),
                    (GLfloat)(k + GRID_SIZE) / (2 * GRID_SIZE + 1), 1.0f);
                glVertex3d(i * GRID_STEP,j * GRID_STEP,k * GRID_STEP);
            }

    glEnd();
    glEndList();
}

void Grid3DScene::drawMyself(CloudViewDialog* /*dialog*/ /*, const Draw3dParameters * parameters*/)
{
    //qDebug() << "Calling Grid3DScene::drawMyself() for" << name;

    if (mGridId == 0)
    {
        qDebug("Grid3DScene was not initialized");
    }
    GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    glCallList(mGridId);
    if (textureState) {
        glEnable(GL_TEXTURE_2D);
    }
}


void Plane3DScene::prepareMesh(CloudViewDialog* /*dialog*/) {

    //qDebug() << "Calling Plane3DScene::prepareMesh() for" << name;

    mPlaneListId = glGenLists(1);
    if (mPlaneListId == 0) {
        qDebug("Plane3DScene failed to create display list");
        return;
    }

    glNewList(mPlaneListId,GL_COMPILE);


    glBegin(GL_LINES);
    glColor3ub(0,255,255);

    const double STEP = 10;
    const int    PLANE_SIZE = 50;

    for (int i = 0; i <= 2 * PLANE_SIZE; i++)
    {
        double x =  PLANE_SIZE * STEP;
        double z =  i * STEP;
        if        (i % 10 == 0) {
            glColor3ub( 128,128,0);
        } else if (i % 2 == 0) {
            glColor3ub( 0,128,128);
        } else {
            glColor3ub( 0, 64, 64);
        }

        glVertex3d(-x, 0, z);
        glVertex3d( x, 0, z);
    }

    for (int j = -PLANE_SIZE; j <= PLANE_SIZE; j++)
    {
        double x = j * STEP;
        double z = 2 * PLANE_SIZE * STEP;

        if        (j % 10 == 0) {
            glColor3ub( 128,128,0);
        } else if (j % 2 == 0) {
            glColor3ub( 0,128,128);
        } else {
            glColor3ub( 0, 64, 64);
        }



        glVertex3d(x, 0, 0);
        glVertex3d(x, 0, z);
    }

    glEnd();
    glDisable(GL_LINE_STIPPLE);
    glEndList();
}

void Plane3DScene::drawMyself(CloudViewDialog* /*dialog*/)
{
    //qDebug() << "Calling Plane3DScene::drawMyself() for" << name;

    if (mPlaneListId == 0)
    {
        qDebug("Grid3DScene was not initialized");
    }
    glDisable(GL_TEXTURE_2D);
    glCallList(mPlaneListId);
   // glEnable(GL_TEXTURE_2D);
}

void CameraScene::prepareMesh(CloudViewDialog * /*dialog*/)
{
}


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

StereoCameraScene::StereoCameraScene(const RectificationResult &result) :
    mStereoPair(result)
{


}

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


