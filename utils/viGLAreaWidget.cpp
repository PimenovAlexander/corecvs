

#include <math.h>

#include "viGLAreaWidget.h"
#include "g12Buffer.h"

ViGLAreaWidget::ViGLAreaWidget(QWidget *parent) :
    QGLWidget(parent)
{
    ui.setupUi(this);
}

void ViGLAreaWidget::initializeGL()
{
    emit askParentInitialize();
}

void ViGLAreaWidget::resizeGL(int width, int height)
{
    emit notifyParentResize(width, height);
}


void ViGLAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit notifyParentMouseMove(event);
}

void ViGLAreaWidget::mousePressEvent(QMouseEvent *event)
{
    emit notifyParentMousePress(event);
}

void ViGLAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    emit notifyParentMouseRelease(event);
}

void ViGLAreaWidget::paintGL()
{
    emit askParentRepaint();
}

void ViGLAreaWidget::wheelEvent ( QWheelEvent * event )
{
//    cout << "ViGLAreaWidget::wheelEvent" << endl;
    emit notifyParentWheelEvent(event);
}

void ViGLAreaWidget::keyPressEvent   ( QKeyEvent * event )
{
//    cout << "ViGLAreaWidget::keyPressEvent" << endl;
    emit notifyParentKeyPressEvent(event);
}

void ViGLAreaWidget::keyReleaseEvent ( QKeyEvent * event )
{
//    cout << "ViGLAreaWidget::keyReleaseEvent" << endl;
    emit notifyParentKeyReleaseEvent(event);
}


ViGLAreaWidget::~ViGLAreaWidget()
{

}
