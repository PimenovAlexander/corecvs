/**
 * \file openGLTools.cpp
 * \brief Add Comment Here
 *
 * \date Nov 19, 2011
 * \author alexander
 */

#include "openGLTools.h"
#include "matrix44.h"

void OpenGLTools::drawArrow(double length, double width)
{
    /*glutSolidCylinder(width / 2.0, length, 10, 10);
    glutSolidCone(width * 1.5, 30, 10, 10);*/

    glBegin(GL_LINES);
        glVertex3d(   0.0,0.0,0.0);
        glVertex3d(length,0.0,0.0);

        glVertex3d(length,0.0,0.0);
        glVertex3d(length - width,  width,0.0);

        glVertex3d(length,0.0,0.0);
        glVertex3d(length - width, -width,0.0);
    glEnd();
}


void OpenGLTools::drawOrts(double /*length*/, double /*width*/)
{
    glColor3ub(255,0,0);
    OpenGLTools::drawArrow(10.0, 1.0);
    glPushMatrix();
    glRotatef(90,0.0f,0.0f,1.0f);
    glColor3ub(0,255,0);
    OpenGLTools::drawArrow(10.0, 1.0);
    glPopMatrix();
    glPushMatrix();
    glRotatef(-90,0.0f,1.0f,0.0f);
    glColor3ub(0,0,255);
    OpenGLTools::drawArrow(10.0, 1.0);
    glPopMatrix();
}

void OpenGLTools::glColorRGB(const RGBColor &color)
{
    //glColor3f(color.r() / 256.0, color.g() / 256.0, color.b() / 256.0);
    glColor3ub(color.r(), color.g(), color.b());
}

void OpenGLTools::glVertexVector3dd(const Vector3dd &p)
{
   // glVertex3d(p.x(), p.y(), p.z());
    glVertex3dv(&(p.x()));
}

void OpenGLTools::glTranslateVector3dd(const Vector3dd &p)
{
    glTranslated(p.x(), p.y(), p.z());
}

void OpenGLTools::glMultMatrixMatrix44(const Matrix44 &p)
{
    glMultMatrixd(p.transposed().element);
}

void OpenGLTools::glMultMatrixMatrix33(const Matrix33 &p)
{
    glMultMatrixd(Matrix44(p).transposed().element);
}



void OpenGLTools::drawWiredBox(const Vector3dd &p1, const Vector3dd &p2)
{
    glBegin(GL_LINES);
    /* One side */
        glVertex3d(p1.x(), p1.y(), p1.z());
        glVertex3d(p1.x(), p2.y(), p1.z());

        glVertex3d(p1.x(), p2.y(), p1.z());
        glVertex3d(p2.x(), p2.y(), p1.z());

        glVertex3d(p2.x(), p2.y(), p1.z());
        glVertex3d(p2.x(), p1.y(), p1.z());

        glVertex3d(p2.x(), p1.y(), p1.z());
        glVertex3d(p1.x(), p1.y(), p1.z());

    /* Other side */
        glVertex3d(p1.x(), p1.y(), p2.z());
        glVertex3d(p1.x(), p2.y(), p2.z());

        glVertex3d(p1.x(), p2.y(), p2.z());
        glVertex3d(p2.x(), p2.y(), p2.z());

        glVertex3d(p2.x(), p2.y(), p2.z());
        glVertex3d(p2.x(), p1.y(), p2.z());

        glVertex3d(p2.x(), p1.y(), p2.z());
        glVertex3d(p1.x(), p1.y(), p2.z());

    /*Connections*/
        glVertex3d(p1.x(), p1.y(), p1.z());
        glVertex3d(p1.x(), p1.y(), p2.z());

        glVertex3d(p1.x(), p2.y(), p1.z());
        glVertex3d(p1.x(), p2.y(), p2.z());

        glVertex3d(p2.x(), p2.y(), p1.z());
        glVertex3d(p2.x(), p2.y(), p2.z());

        glVertex3d(p2.x(), p1.y(), p2.z());
        glVertex3d(p2.x(), p1.y(), p1.z());
    glEnd();
}


void OpenGLTools::drawWiredCamera()
{
    drawWiredBox(Vector3dd(-5.0,-5.0,-15.0), Vector3dd(5.0,5.0,-5.0));

    glBegin(GL_LINES);

    int CIRCLE_STEPS = 16;
    for (int i = 0; i < CIRCLE_STEPS; i++)
    {
        double angle1 = (i / (double)CIRCLE_STEPS * 2.0 * M_PI);
        double angle2 = (((i + 1) % CIRCLE_STEPS) / (double)CIRCLE_STEPS * 2.0 * M_PI);

        double sa1 = sin(angle1);
        double ca1 = cos(angle1);

        double sa2 = sin(angle2);
        double ca2 = cos(angle2);

        double r1 = 2.5;
        double r2 = 4.5;

        glVertex3d(r1 * sa1, r1 * ca1 ,-5.0);
        glVertex3d(r1 * sa2, r1 * ca2 ,-5.0);

        glVertex3d(r2 * sa1, r2 * ca1 ,-0.0);
        glVertex3d(r2 * sa2, r2 * ca2 ,-0.0);

        glVertex3d(r1 * sa1, r1 * ca1 ,-5.0);
        glVertex3d(r2 * sa1, r2 * ca1 ,-0.0);
    }
    glEnd();

}

void OpenGLTools::drawDiamond(const Vector3dd &p, const double r)
{
    glBegin(GL_TRIANGLES);
        /**/
        glVertex3d(p.x() + r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y() + r, p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() + r);
        /**/
        glVertex3d(p.x() + r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() + r);
        glVertex3d(p.x()    , p.y() - r, p.z()    );
        /**/
        glVertex3d(p.x() - r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y() - r, p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() + r);
        /**/
        glVertex3d(p.x() - r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() + r);
        glVertex3d(p.x()    , p.y() + r, p.z()    );
        /**/
/* And bottom part */
        /**/
        glVertex3d(p.x() + r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y() + r, p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() - r);
        /**/
        glVertex3d(p.x() + r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() - r);
        glVertex3d(p.x()    , p.y() - r, p.z()    );
        /**/
        glVertex3d(p.x() - r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y() - r, p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() - r);
        /**/
        glVertex3d(p.x() - r, p.y()    , p.z()    );
        glVertex3d(p.x()    , p.y()    , p.z() - r);
        glVertex3d(p.x()    , p.y() + r, p.z()    );
        /**/
    glEnd();
}



void OpenGLTools::gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    double fovyRad = degToRad(fovy);
    double f = 1.0 / tan(fovyRad / 2);
    double zDepth = zNear - zFar;
    Matrix44 matrix (
        f / aspect, 0 ,             0          ,            0            ,
        0         , f ,             0          ,            0            ,
        0         , 0 , (zFar + zNear) / zDepth, 2 *zFar * zNear / zDepth,
        0         , 0 ,            -1          ,            0
    );

    glMultMatrixMatrix44(matrix);
}

void OpenGLTools::gluPerspectivePosZ(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar)
{
    double fovYRad = degToRad(fovy);
    Matrix44 matrix = Matrix44::Frustum(fovYRad, aspect, zNear, zFar);
    glMultMatrixMatrix44(matrix);
}
