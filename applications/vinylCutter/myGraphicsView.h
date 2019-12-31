#pragma once

#include <fstream>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QDebug>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#include <gcodeHandler.h>
#include <core/fileformats/gcodeLoader.h>


class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	MyGraphicsView(QWidget *parent = nullptr);
	~MyGraphicsView();
    void setImage(QImage image);
    int getXSize();
    int getYSize();
    int getXShift();
    int getYShift();
    void setXShift(int shift);
    void setYShift(int shift);

private:
    GcodeHandler *gcodeHandler;
    QWidget *parent = nullptr;
    bool isResized;
	bool isLandscape;
    QGraphicsScene *scene;
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    QPixmap image;
    QGraphicsPixmapItem *pixmap;
    int xImageSize;
    int yImageSize;
    int xImageShift;
    int yImageShift;
};
