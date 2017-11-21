#ifndef OPENGLTOOLS_H_
#define OPENGLTOOLS_H_
/**
 * \file openGLTools.h
 * \brief Add Comment Here
 *
 * \date Nov 19, 2011
 * \author alexander
 */

#include "QtOpenGL/QGLWidget"

#include "core/buffers/rgb24/rgbColor.h"
#include "core/math/matrix/matrix44.h"
#include "core/math/matrix/matrix33.h"
#include "core/buffers/rgb24/abstractPainter.h"

using namespace corecvs;

/**
 *  With new style Qt this class seem to become deprecated
 **/
class OpenGLTools
{
public:

    static void drawArrow(double length, double width);
    static void drawOrts(double length, double width);

    static void glColorRGB(const RGBColor &color);
    static void glVertexVector3dd(const Vector3dd &p);
    static void glTranslateVector3dd(const Vector3dd &p);

    static void glMultMatrixMatrix44(const Matrix44 &p);
    static void glMultMatrixMatrix33(const Matrix33 &p);

    static Matrix44 glGetProjectionMatrix();
    static Matrix44 glGetModelViewMatrix();

    static void drawWiredBox(const Vector3dd &p1, const Vector3dd &p2);
    static void drawWiredCamera();

    static void drawDiamond(const Vector3dd &p1, const double r);

    /**
     *  Equivalent to old function gluPerspective
     **/
    static void gluPerspective(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);

    /**
     *  Same as above except for looking in positive Z direction
     **/
    static void gluPerspectivePosZ(GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);


    class GLWrapper {
    public:
        typedef RGBColor InternalElementType;

        void drawLine(int x1, int y1, int x2, int y2, const RGBColor &color )
        {
            glColorRGB(color);
            glBegin(GL_LINES);
                glVertex2d(x1,y1);
                glVertex2d(x2,y2);
            glEnd();
        }
    };

};

typedef AbstractPainter<OpenGLTools::GLWrapper> GLPainter;

#endif /* OPENGLTOOLS_H_ */
