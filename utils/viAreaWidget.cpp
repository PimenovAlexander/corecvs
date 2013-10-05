#include "viAreaWidget.h"

ViAreaWidget::ViAreaWidget(QWidget *parent, bool forwardEvents)
    : QWidget(parent),
      mForwardEvents(forwardEvents)
{
    ui.setupUi(this);
}

void ViAreaWidget::paintEvent(QPaintEvent *event)
{
    if (!mForwardEvents) return;
    emit askParentRepaint(event, this);
}

void ViAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!mForwardEvents) return;
    emit notifyParentMouseMove(event);
}

void ViAreaWidget::mousePressEvent(QMouseEvent *event)
{
    if (!mForwardEvents) return;
    emit notifyParentMouseMove(event);
    emit notifyParentMousePress(event);
}

void ViAreaWidget::resizeEvent ( QResizeEvent * event )
{
    if (!mForwardEvents) return;
    emit notifyParentResize(event);
}

void ViAreaWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (!mForwardEvents) return;
    emit notifyParentMouseMove(event);
}

void ViAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!mForwardEvents) return;
    emit notifyParentMouseRelease(event);
}

void ViAreaWidget::wheelEvent ( QWheelEvent * event )
{
    if (!mForwardEvents) return;
    emit notifyParentWheelEvent(event);
}

void ViAreaWidget::keyPressEvent   ( QKeyEvent * event )
{
    if (!mForwardEvents) return;
    emit notifyKeyPressEvent(event);
}

void ViAreaWidget::keyReleaseEvent ( QKeyEvent * event )
{
    if (!mForwardEvents) return;
    emit notifyKeyReleaseEvent(event);
}


ViAreaWidget::~ViAreaWidget()
{
}
