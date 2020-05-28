#include "opengl/openGLTools.h"
#include "billboardCaption3DScene.h"

BillboardCaption3DScene::BillboardCaption3DScene()
{

}

void BillboardCaption3DScene::drawMyself(CloudViewDialog *dialog)
{
    /*Caption drawing*/

    OpenGLTools::GLWrapper wrapper;
    GLPainter painter(&wrapper);

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

    double size = mParameters.fontSize() / 25.0;

    for (auto caption: mLabels)
    {
        /*
        QString text = QString("[%1, %2, %3]")
            .arg(centralPoint.x(), 0, 'f', 2).arg(centralPoint.y(), 0, 'f', 2).arg(centralPoint.z(), 0, 'f', 2);
        */
        QString text = QString::fromStdString(caption.second);
        Vector3dd labelPos = glMatrix * caption.first;

        if ((!dialog->isOrtho((CloudViewDialog::CameraType)dialog->mUi.cameraTypeBox->currentIndex())) && (labelPos.z() < 0)) {
            continue;
        }

        glPushMatrix();
            glTranslated(labelPos[0] * width / 2.0, labelPos[1] * height / 2.0, 0.0);
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
        glPopMatrix();
    }

    /*Restore old matrices */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();



}

BillboardCaption3DScene::~BillboardCaption3DScene()
{

}

