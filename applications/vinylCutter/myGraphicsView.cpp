#include <QDebug>
#include <QMimeData>
#include <QFileDialog>
#include <core/fileformats/gcodeLoader.h>
#include "myGraphicsView.h"
#include "./ui_mainWindow.h"


using namespace std;
using namespace corecvs;

MyGraphicsView::MyGraphicsView(QWidget *parent)
        : QGraphicsView(parent) {
    scene = new QGraphicsScene(this);
    this->setScene(scene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    setAcceptDrops(true);

    int xImageSize = 0;
    int yImageSize = 0;
    int xImageShift = 0;
    int yImageShift = 0;

    gcodeHandler = GcodeHandler::getInstance();
}

MyGraphicsView::~MyGraphicsView() {}

void MyGraphicsView::dragEnterEvent(QDragEnterEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList <QUrl> urlList = mimeData->urls();
        QString filePath = mimeData->urls().at(0).path();
        QFileInfo f(filePath);
        QString ext = f.suffix();

        if (ext == "gcode") event->acceptProposedAction();
    }
}

void MyGraphicsView::dragMoveEvent(QDragMoveEvent *event) {
    event->acceptProposedAction();
}

void MyGraphicsView::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        scene->clear();

        QList <QUrl> urlList = mimeData->urls();
        if (urlList.size() > 1) {
            qDebug() << "Too many files. Only the first file will be handled";
        }
        QString filePath = mimeData->urls().at(0).path();
        importGcode(filePath);

    } else qDebug() << "ERROR: Null file url";
}

void MyGraphicsView::importGcode(QString filePath) {
    gcodeHandler->loadGcode(filePath.toStdString());
    Vector2dd shift = gcodeHandler->applyShift(0.0, 0.0);
    ui->xShift->setValue(shift.x());
    ui->yShift->setValue(shift.y());
    initXShift = shift.x();
    initYShift = shift.y();
    gcodeHandler->drawMesh(scene);
    this->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);

    connect(ui->xShift, SIGNAL(valueChanged(double)), this, SLOT(setXShift(double)));
    connect(ui->yShift, SIGNAL(valueChanged(double)), this, SLOT(setYShift(double)));
    connect(ui->offset, SIGNAL(valueChanged(double)), this, SLOT(setBladeOffset(double)));
    connect(ui->touchZ, SIGNAL(valueChanged(double)), this, SLOT(setTouchZ(double)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(gcodeExportTriggered()));
}

void MyGraphicsView::setUI(Ui::MainWindow *main_ui) {
    ui = main_ui;
}

void MyGraphicsView::setXShift(double shift) {
    xImageShift = shift;
}

void MyGraphicsView::setYShift(double shift) {
    yImageShift = shift;
}

void MyGraphicsView::setBladeOffset(double val) {
    bladeOffset = val;
}

void MyGraphicsView::setTouchZ(double val) {
    touchZ = val;
}

void MyGraphicsView::gcodeExportTriggered() {
    gcodeHandler->applyShift(xImageShift - initXShift, yImageShift - initYShift);
    gcodeHandler->compensateDragKnife(bladeOffset, touchZ);

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save File"), "/home/vinylCutter.gcode",
                                                    tr("*.gcode"));
    gcodeHandler->saveGcode(filePath.toStdString());
}
