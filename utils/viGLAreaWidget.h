#ifndef VIGLAREA_H
#define VIGLAREA_H

#include <QMouseEvent>
#include <QWheelEvent>
#define __XMLDocument_FWD_DEFINED__  // to omit conflict "msxml.h:3376: using typedef-name 'XMLDocument' after 'class'"
#include <QtOpenGL/QGLWidget>

#include <QTimer>

#include "ui_viGLAreaWidget.h"


class ViGLAreaWidget : public QGLWidget
{
    Q_OBJECT

public:
    ViGLAreaWidget(QWidget *parent = 0);
    ~ViGLAreaWidget();

signals:
    void askParentInitialize();
    void askParentRepaint();
    void notifyParentMouseMove(QMouseEvent* event);
    void notifyParentMousePress(QMouseEvent* event);
    void notifyParentMouseRelease(QMouseEvent* event);
    void notifyParentWheelEvent ( QWheelEvent * event );
    void notifyParentResize(int width, int height);
//    void notifyParentResize( QResizeEvent *event );
    void notifyParentKeyPressEvent   ( QKeyEvent * event );
    void notifyParentKeyReleaseEvent ( QKeyEvent * event );

public slots:
    void scheduleUpdate();
    virtual void updateGL() override;
    void setScheduleDelay(int delay);

private:
    Ui_ViGLAreaWidgetClass ui;

    virtual void initializeGL();
    virtual void resizeGL(int width, int height);
    virtual void paintGL();
    virtual void mouseMoveEvent(QMouseEvent * event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void wheelEvent ( QWheelEvent * event );
//    virtual void resizeEvent (QResizeEvent * event);
    virtual void keyPressEvent   ( QKeyEvent * event );
    virtual void keyReleaseEvent ( QKeyEvent * event );

private:

    int mScheduleDelay;
    QTimer mTimer;

};

#endif // VIGLAREA_H
