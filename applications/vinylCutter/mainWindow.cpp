#include "mainWindow.h"
#include <myGraphicsView.h>
#include "./ui_mainWindow.h"
#include <QDir>


using namespace corecvs;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QGraphicsView *view = ui->graphicsView;
    background = new QGraphicsScene(ui->graphicsView);
    ui->graphicsView->setScene(background);

    MyGraphicsView *myView = ui->graphicsView_2;
    myView->setUI(ui);

    gcodeHandler = GcodeHandler::getInstance();

    this->setBackground();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openGcodeTriggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Gcode File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));

    if (!filePath.isEmpty()) {
        return;
    }
}

void MainWindow::gcodeExportTriggered() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));

//    gCodeExportHandler(filePath);
}

void MainWindow::setBackground() {
    QString filePath = QDir::currentPath() + "/applications/vinylCutter/resources/background.png";

    qDebug() << filePath;

    QImage image;
    image.load(filePath);

    QPixmap pixmap = QPixmap::fromImage(image);
    auto *item = background->addPixmap(pixmap);
    qDebug() << item->offset();
    item->setOffset(-95, -80);
}

//void MainWindow::gCodeExportHandler(QString filePath) {
//    compensator.compensateDragKnife(program, offset, touchZ);
//
//    ofstream ofile;
//    ofile.open(outfile, ios::out);
//    if (ofile.fail())
//    {
//        SYNC_PRINT(("Can't open output gcode file <%s>\n", outfile.c_str()));
//        return 1;
//    }
//
//    loader.saveGcode(ofile, compensator.result);
//
//    ofile.close();
//}
