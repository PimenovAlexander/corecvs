#include "helper3DScenes.h"

#include <openGLTools.h>

const int Grid3DScene::GRID_SIZE = 5;
const int Grid3DScene::GRID_STEP = 250;


void Grid3DScene::render()
{
    int oldPointSize = 1;
    glGetIntegerv(GL_POINT_SIZE, &oldPointSize);
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
    glPointSize(oldPointSize);
}

void Grid3DScene::prepareMesh(CloudViewDialog* dialog)
{
    //qDebug() << "Calling Grid3DScene::prepareMesh() for" << name;

    //dialog->mUi.widget->makeCurrent();
    //QOpenGLFunctions_4_5_Core &glFuncs = *(dialog->mUi.widget->context()->versionFunctions<QOpenGLFunctions_4_5_Core>());


    mGridId = glGenLists(1);
    if (mGridId == 0) {
        qDebug("Grid3DScene failed to create display list");
        return;
    }

    glNewList(mGridId, GL_COMPILE);
        render();
    glEndList();
}

void Grid3DScene::drawMyself(CloudViewDialog* /*dialog*/ /*, const Draw3dParameters * parameters*/)
{
    //qDebug() << "Calling Grid3DScene::drawMyself() for" << name;
    GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
    glShadeModel(GL_FLAT);

    if (mGridId == 0)
    {
        qDebug("Grid3DScene was not initialized. Falling back to slow version");
        render();
    } else {
        glCallList(mGridId);
    }
    if (textureState) {
        glEnable(GL_TEXTURE_2D);
    }
}


void Plane3DScene::render()
{
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
}

void Plane3DScene::prepareMesh(CloudViewDialog* /*dialog*/)
{
    //qDebug() << "Calling Plane3DScene::prepareMesh() for" << name;

    mPlaneListId = glGenLists(1);
    if (mPlaneListId == 0) {
        qDebug("Plane3DScene failed to create display list");
        return;
    }

    glNewList(mPlaneListId,GL_COMPILE);
        render();
    glDisable(GL_LINE_STIPPLE);
    glEndList();
}

void Plane3DScene::drawMyself(CloudViewDialog* /*dialog*/)
{
    //qDebug() << "Calling Plane3DScene::drawMyself() for" << name;

    if (mPlaneListId == 0)
    {
        qDebug("Plane3DScene was not initialized");
    }
    GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);

    glCallList(mPlaneListId);

    if (textureState) {
        glEnable(GL_TEXTURE_2D);
    }
}

void Plane3DGeodesicScene::render()
{
    glBegin(GL_LINES);
    glColor3ub(0,255,255);

    const double STEP = 10;
    const int    PLANE_SIZE = 50;

    // South to North lines
    for (int i = -PLANE_SIZE; i <= PLANE_SIZE; i++)
    {
        double x =  PLANE_SIZE * STEP;
        double y =  i * STEP;
        if        (i % 10 == 0) {
            glColor3ub( 128,128,0);
        } else if (i % 2 == 0) {
            glColor3ub( 0,128,128);
        } else {
            glColor3ub( 0, 64, 64);
        }
        glVertex3d(-x, y, 0);
        glVertex3d( x, y, 0);
    }
    // West to East lines
    for (int i = -PLANE_SIZE; i <= PLANE_SIZE; i++)
    {
        double x =  i * STEP;
        double y =  PLANE_SIZE * STEP;
        if        (i % 10 == 0) {
            glColor3ub( 128,128,0);
        } else if (i % 2 == 0) {
            glColor3ub( 0,128,128);
        } else {
            glColor3ub( 0, 64, 64);
        }
        glVertex3d( x, -y, 0);
        glVertex3d( x,  y, 0);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    glPushMatrix();
    OpenGLTools::glMultMatrixMatrix33(Matrix33::MirrorXY());
    OpenGLTools::GLWrapper wrapper;
    GLPainter painter(&wrapper);
    painter.drawGlyph(0,  PLANE_SIZE * STEP - 20, 'N', RGBColor::Blue(), 5);
    painter.drawGlyph(0, -PLANE_SIZE * STEP, 'S', RGBColor::Red(), 5);
    painter.drawGlyph(PLANE_SIZE * STEP,  0, 'E', RGBColor::Yellow(), 5);
    painter.drawGlyph(-PLANE_SIZE * STEP - 20,  0, 'W', RGBColor::Cyan(), 5);
    glPopMatrix();
}

void Plane3DGeodesicScene::prepareMesh(CloudViewDialog* /*dialog*/)
{
    //qDebug() << "Calling Plane3DGeodesicScene::prepareMesh() for" << name;

    mPlaneListId = glGenLists(1);
    if (mPlaneListId == 0) {
        qDebug("Plane3DGeodesicScene failed to create display list");
        return;
    }

    glNewList(mPlaneListId,GL_COMPILE);
        render();
    glEndList();
}

void Plane3DGeodesicScene::drawMyself(CloudViewDialog* /*dialog*/)
{
    //qDebug() << "Calling Plane3DScene::drawMyself() for" << name;

    GLboolean textureState = glIsEnabled(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);

    if (mPlaneListId == 0)
    {
        qDebug("Plane3DGeodesicScene was not initialized");
        render();
    } else {
        glCallList(mPlaneListId);
    }

    if (textureState) {
        glEnable(GL_TEXTURE_2D);
    }
}
