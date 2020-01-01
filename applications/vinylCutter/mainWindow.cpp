#include "mainWindow.h"
#include "./ui_mainWindow.h"


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

    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openGcodeTriggered()));

    this->setBackground();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::openGcodeTriggered() {
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Gcode File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));

    if (!filePath.isEmpty()) {
        ui->graphicsView_2->importGcode(filePath);
    }
}

void MainWindow::setBackground() {
    QString filePath = QDir::currentPath() + "/applications/vinylCutter/resources/background.png";
    QImage image;
    image.load(filePath);

    QPixmap pixmap = QPixmap::fromImage(image);
    auto *item = background->addPixmap(pixmap);
    item->setOffset(-95, -80);
}