#include "mainWindow.h"
#include "./ui_mainWindow.h"


using namespace corecvs;
using namespace std;

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    gcodeHandler = GcodeHandler::getInstance();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openGcodeTriggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Gcode File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));

//    if (!filePath.isEmpty()) {
//        ui->graphicsView->setImage(filePath);
//    }
}

void MainWindow::gcodeExportTriggered() {
    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));

//    gCodeExportHandler(filePath);
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
