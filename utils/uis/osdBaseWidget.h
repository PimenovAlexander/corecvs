#pragma once
#include <QtCore/QTimer>
#include <QtGui/QWidget>


class OSDBaseWidget : public QWidget
{
    Q_OBJECT
public:
    OSDBaseWidget(QWidget *parent = 0);
    virtual ~OSDBaseWidget();

    void paintEvent(QPaintEvent *);
    void offscreenChanged(QImage *newOffscreen);

    void keyPressEvent(QKeyEvent * event);

signals:
    void resetState();

private:
    void runOnTopCommand();

    QImage *offscreen;
};
