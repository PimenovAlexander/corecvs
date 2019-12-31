#pragma once

#include <fstream>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMainWindow>
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



namespace Ui { class MyGraphicsView; class MainWindow; }

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	MyGraphicsView(QWidget *parent = nullptr);
	~MyGraphicsView();
    void setImage(QImage image);
    double getXShift();
    double getYShift();
    double getBladeOffset();
    double getTouchZ();
    void setUI(Ui::MainWindow *main_ui);
    void importGcode(QString filePath);

public slots:
    void setXShift(double shift);
    void setYShift(double shift);;
    void setBladeOffset(double val);;
    void setTouchZ(double val);
    void gcodeExportTriggered();

private:
    Ui::MainWindow *ui;
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
    double xImageShift;
    double yImageShift;
    double bladeOffset;
    double touchZ;
    double initXShift = 0;
    double initYShift = 0;
};
