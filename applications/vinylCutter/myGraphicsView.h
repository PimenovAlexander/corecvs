#pragma once

#include <fstream>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QGraphicsView>
#include <QGraphicsScene>
#include "gcodeHandler.h"


namespace Ui { class MyGraphicsView; class MainWindow; }

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT

public:
	MyGraphicsView(QWidget *parent = nullptr);
	~MyGraphicsView();
    void setImage(QImage image);
    void setUI(Ui::MainWindow *main_ui);
    void importGcode(QString filePath);

public slots:
    void setXShift(double shift);
    void setYShift(double shift);;
    void setTouchZ(double val);
    void setBladeOffset(double val);;
    void gcodeExportTriggered();

private:
    Ui::MainWindow *ui;
    GcodeHandler *gcodeHandler;
    QWidget *parent = nullptr;
    QGraphicsScene *scene;
    void dropEvent(QDropEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    double xImageShift;
    double yImageShift;
    double initXShift = 0;
    double initYShift = 0;
    double bladeOffset;
    double touchZ;
};
