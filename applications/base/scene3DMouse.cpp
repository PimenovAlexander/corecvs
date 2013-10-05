/**
 * \file scene3D.cpp
 * \brief Add Comment Here
 *
 * \date Dec 8, 2011
 * \author alexander
 */
#include <QtOpenGL/QGLWidget>

#include "scene3DMouse.h"
#include "cloudViewDialog.h"
#include "opengl/openGLTools.h"



/* Scene for ViMouse */
Scene3DMouse::Scene3DMouse() :
	cloud(NULL)
{}

void Scene3DMouse::drawMyself(CloudViewDialog *dialog)
{
    Scene3D::drawMyself(dialog);

    if (cloud == NULL) {
        return;
    }

    double redDist  = mParameters.redDist();
    double blueDist = mParameters.blueDist();
    double flowZoom = mParameters.flowZoom();
    int pointColorType = mParameters.pointColorType();
    int flowColorType  = mParameters.flowColorType();

    double    zoom  = dialog->mCameraZoom;
    Matrix44  camera = dialog->mCamera;

    if (pointColorType != ViMouse3dStereoStyle::NONE)
    {
        glBegin(GL_POINTS);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        for (unsigned i = 0; i < cloud->size(); i++)
        {
            SwarmPoint p = cloud->operator[](i);
            Vector3dd point = p.point;

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
            OpenGLTools::glVertexVector3dd(point);
        }
        glEnd();
    }


    if (flowColorType == ViMouse3dFlowStyle::WHITE || flowColorType == ViMouse3dFlowStyle::HEAT)
    {
        /* Draw Flow */
        glBegin(GL_LINES);
        glLineWidth(1.0);

        if (flowColorType == ViMouse3dFlowStyle::WHITE) {
            glColor4f(1.0, 1.0, 1.0, 1.0);
        }

        for (unsigned i = 0; i < cloud->size(); i++)
        {
            SwarmPoint p  = cloud->operator[](i);
            Vector3dd point = p.point;
            Vector3dd flow  = p.speed;
            Vector3dd point1 = point - flow * flowZoom + Vector3dd(0.00001);

            if (flowColorType == ViMouse3dFlowStyle::HEAT)
            {
                OpenGLTools::glColorRGB(
                    RGBColor::rainbow1(lerpLimit(0.0, 1.0, flow.l1Metric(), 15.0, 100.0))
                );
            }

            OpenGLTools::glVertexVector3dd(point);
            OpenGLTools::glVertexVector3dd(point1);
        }
        glEnd();
    }

    if (flowColorType == ViMouse3dFlowStyle::BY_FLAG)
    {
        glBegin(GL_LINES);
        glLineWidth(1.0);

            for (unsigned i = 0; i < cloud->size(); i++)
            {
                SwarmPoint &p  = cloud->operator[](i);

                if (p.flag != 1 && p.flag != 2)
                    continue;

                if      (p.flag == SwarmPoint::IS_6D)
                     glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                else if (p.flag == SwarmPoint::USER_1)
                     glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

                Vector3dd point = p.point;
                Vector3dd flow  = p.speed;
                Vector3dd point1 = point - flow * flowZoom + Vector3dd(0.00001);

                OpenGLTools::glVertexVector3dd(point);
                OpenGLTools::glVertexVector3dd(point1);
            }
        glEnd();
    }
}

/**
 * Format description
 * http://pointclouds.org/documentation/tutorials/pcd_file_format.php
 **/
void Scene3DMouse::dumpPCD(ostream &out)
{
   /* VERSION .7
    FIELDS x y z rgb
    SIZE 4 4 4 4
    TYPE F F F F
    COUNT 1 1 1 1
    WIDTH 213
    HEIGHT 1
    VIEWPOINT 0 0 0 1 0 0 0
    POINTS 213
    DATA ascii
    0.93773 0.33763 0 4.2108e+06*/

    out << "# .PCD v.7 - Point Cloud Data file format\n";
    out << "VERSION .7\n";
    out << "FIELDS x y z rgb\n";
    out << "SIZE  4 4 4 4\n";
    out << "TYPE  F F F U\n";
    out << "COUNT 1 1 1 1\n";
    out << "WIDTH " << cloud->size() << "\n";
    out << "HEIGHT 1\n";
    out << "VIEWPOINT 0 0 0 1 0 0 0\n";
    out << "POINTS " << cloud->size() << "\n";
    out << "DATA ascii\n";

    for (unsigned i = 0; i < cloud->size(); i++)
    {
       SwarmPoint &p  = cloud->operator[](i);
       out << (p.point.x() / 1000.0)  << " " << (p.point.y() / 1000.0) << " " << (p.point.z() / 1000.0) << " "
           << p.color.color() <<  endl;
    }

}


/**
 * Format description
 * http://local.wasp.uwa.edu.au/~pbourke/dataformats/ply/
 **/
void Scene3DMouse::dumpPLY(ostream &out)
{

    out << "ply" << endl;
    out << "format ascii 1.0" << endl;
    out << "comment made by ViMouse software" << endl;
    out << "comment This file is a saved stereo-reconstruction" << endl;
    out << "element vertex " << cloud->size() << endl;
    out << "property float x" << endl;
    out << "property float y" << endl;
    out << "property float z" << endl;
    out << "property uchar red" << endl;
    out << "property uchar green" << endl;
    out << "property uchar blue" << endl;
    out << "end_header" << endl;
    for (unsigned i = 0; i < cloud->size(); i++)
    {
        SwarmPoint &p  = cloud->operator[](i);
        double x = p.point.x() / 100.0;
        double y = p.point.y() / 100.0;
        double z = p.point.z() / 100.0;

        double limit = 1000;

        if (x < -limit) x = -limit;
        if (y < -limit) y = -limit;
        if (z < -limit) z = -limit;

        if (x > limit) x = limit;
        if (y > limit) y = limit;
        if (z > limit) z = limit;


       out << x << " "
           << y << " "
           << z << " "
           << (unsigned)(p.color.r()) << " "
           << (unsigned)(p.color.g()) << " "
           << (unsigned)(p.color.b()) << endl;
    }

}

Scene3DMouse::~Scene3DMouse()
{
    delete_safe(cloud);
}

