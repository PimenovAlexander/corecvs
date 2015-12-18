/**
 * \file pointScene.cpp
 *
 * \date Oct 13, 2013
 **/
#include <QtOpenGL/QGLWidget>

#include "pointScene.h"
#include "openGLTools.h"

void PointScene::prepareMesh(CloudViewDialog *dialog)
{

}

void PointScene::drawMyself(CloudViewDialog *dialog)
{
    Scene3D::drawMyself(dialog);

    glBegin(GL_POINTS);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    for (unsigned i = 0; i < scene.size(); i++)
    {
        Point p = scene[i];
        Vector3dd point = p;
#if 0
        switch (pointColorType)
        {
        case ViMouse3dStereoStyle::GREY_VALUE:
            OpenGLTools::glColorRGB(p.color);
            break;
        case ViMouse3dStereoStyle::Z_COORDINATE:
            OpenGLTools::glColorRGB(RGBColor::rainbow1(
                lerpLimit(1.0, 0.0, p.point.z(), redDist, blueDist)
            ));
            break;
        case ViMouse3dStereoStyle::Y_COORDINATE:
            OpenGLTools::glColorRGB(RGBColor::rainbow1(
                lerpLimit(1.0, 0.0, p.point.y(), redDist, blueDist)
            ));
            break;
        case ViMouse3dStereoStyle::BY_FLAG:
            if        (p.flag == SwarmPoint::IS_3D) {
                OpenGLTools::glColorRGB(p.color);
            } else if (p.flag == SwarmPoint::IS_6D) {
                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            } else {
                glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            }
            break;
        case ViMouse3dStereoStyle::BY_CLUSTER:
            {
               RGBColor color(71123 * p.cluster);
               OpenGLTools::glColorRGB(color);
            }
            break;
        default:
        case ViMouse3dStereoStyle::DISTANCE:
            Vector3dd projected = (Matrix44::Scale(zoom) * camera * point);
            OpenGLTools::glColorRGB(RGBColor::rainbow1(
                lerpLimit(1.0, 0.0, projected.z(), redDist, blueDist)
            ));
            break;

        }
#endif
        OpenGLTools::glColorRGB(RGBColor(point.x(),point.y(), point.z()));
        OpenGLTools::glVertexVector3dd(point);
    }
    glEnd();

}


void EllApproxScene::prepareMesh(CloudViewDialog *dialog)
{

}

void EllApproxScene::drawMyself(CloudViewDialog *dialog)
{
    Scene3D::drawMyself(dialog);

    /*mEllip.getEllipseParameters();*/
    //qDebug("%p: Size is: %d", this, mEllip.mAxes.size());
    if (mEllip.mAxes.size() != 0)
    {
     glBegin(GL_LINES);
        OpenGLTools::glColorRGB(RGBColor::Red());
        OpenGLTools::glVertexVector3dd(center);
        OpenGLTools::glVertexVector3dd(center + mEllip.mAxes[0] * 100 /*mEllip.mValues[0]*/) ;

        OpenGLTools::glColorRGB(RGBColor::Green());
        OpenGLTools::glVertexVector3dd(center );
        OpenGLTools::glVertexVector3dd(center + mEllip.mAxes[1] * 100 /*mEllip.mValues[1]*/);

        OpenGLTools::glColorRGB(RGBColor::Blue());
        OpenGLTools::glVertexVector3dd(center);
        OpenGLTools::glVertexVector3dd(center + mEllip.mAxes[2] * 100 /*mEllip.mValues[2]*/);
      glEnd();
    }
}

