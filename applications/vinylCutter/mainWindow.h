#ifndef VINYLCUTTER_H
#define VINYLCUTTER_H

#include <string>
#include <vector>
#include <fstream>
#include <QMainWindow>
#include <QPixmap>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QDir>

#include <gcodeHandler.h>
#include "core/fileformats/svgLoader.h"
#include "core/geometry/polygons.h"
#include <core/fileformats/gcodeLoader.h>

namespace Ui { class MainWindow; }

using namespace corecvs;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:

private:
    GcodeHandler *gcodeHandler;
    void openGcodeTriggered();
    void gcodeExportTriggered();
//    void gCodeExportHandler(QString filePath);
    QGraphicsScene *scene;
    Ui::MainWindow *ui;
};



#endif // VINYLCUTTER_H
