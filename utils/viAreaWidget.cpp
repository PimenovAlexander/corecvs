
#include <QPainter>
#include <QPaintEvent>

#include "viAreaWidget.h"
#include "global.h"

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




void ImageWidget::setImage(QImage *newImage)
{
    QImage *oldImage = image;
    image = newImage;
    delete_safe(oldImage);
    this->resize(image->size());
    this->update();

}

void ImageWidget::setSizeToImage()
{
    if (image != NULL && (!image->isNull()))
    {
        setMinimumWidth (image->width());
        setMinimumHeight(image->height());
    }

}

void ImageWidget::paintEvent(QPaintEvent * event)
{
    QPainter p(this);

    p.setBrush(Qt::green);
    p.drawRect(event->rect());

    if (image != NULL)
        p.drawImage(QPoint(0,0), *image);


    ViAreaWidget::paintEvent(event);
}
