#pragma once

#include <QtGui/QWidget>
#include "ui_viAreaWidget.h"

class ViAreaWidget : public QWidget
{
    Q_OBJECT

signals:
    void askParentRepaint(QPaintEvent *event, QWidget *who);
    void notifyParentMouseMove(QMouseEvent* event);
    void notifyParentMousePress(QMouseEvent* event);
    void notifyParentMouseRelease(QMouseEvent* event);
    void notifyParentWheelEvent ( QWheelEvent * event );
    void notifyParentResize( QResizeEvent *event );
    void notifyKeyPressEvent   ( QKeyEvent * event );
    void notifyKeyReleaseEvent ( QKeyEvent * event );


public:
    ViAreaWidget(QWidget *parent = 0, bool forwardEvents = true);
    ~ViAreaWidget();

    virtual void paintEvent           (QPaintEvent  *event);
    virtual void mouseMoveEvent       (QMouseEvent  *event);
    virtual void mousePressEvent      (QMouseEvent  *event);
    virtual void mouseReleaseEvent    (QMouseEvent  *event);
    virtual void mouseDoubleClickEvent(QMouseEvent  *event);
    virtual void wheelEvent           (QWheelEvent  *event);
    virtual void resizeEvent          (QResizeEvent *event);
    virtual void keyPressEvent        (QKeyEvent    *event);
    virtual void keyReleaseEvent      (QKeyEvent    *event);



private:
    Ui_ViAreaWidgetClass ui;
    bool mForwardEvents;
};
