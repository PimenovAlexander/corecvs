#ifndef VINYLCUTTER_H
#define VINYLCUTTER_H

#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QDir>
#include <core/fileformats/gcodeLoader.h>
#include "gcodeHandler.h"


namespace Ui { class MainWindow; }

using namespace corecvs;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    ~MainWindow();
    MainWindow(QWidget *parent = nullptr);
    void setBackground();

private slots:
    void openGcodeTriggered();

private:
    Ui::MainWindow *ui;
    GcodeHandler *gcodeHandler;
    QGraphicsScene *background;
};

#endif // VINYLCUTTER_H
