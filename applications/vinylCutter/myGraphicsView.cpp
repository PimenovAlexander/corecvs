#include "myGraphicsView.h"
#include "./ui_mainWindow.h"

using namespace std;
using namespace corecvs;

MyGraphicsView::MyGraphicsView(QWidget *parent)
        : QGraphicsView(parent) {
    scene = new QGraphicsScene(this);
//    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setScene(scene);

    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::HighQualityAntialiasing);
    setTransformationAnchor(AnchorUnderMouse);

    isResized = false;
    isLandscape = false;
    setAcceptDrops(true);

    int xImageSize = 0;
    int yImageSize = 0;
    int xImageShift = 0;
    int yImageShift = 0;
//    setDragMode(QGraphicsView::ScrollHandDrag);

    gcodeHandler = GcodeHandler::getInstance();
}

MyGraphicsView::~MyGraphicsView() {
}


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
//    qDebug() << "drag move";
    event->acceptProposedAction();
}

void MyGraphicsView::dropEvent(QDropEvent *event) {
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList <QUrl> urlList = mimeData->urls();
        if (urlList.size() > 1) {
            qDebug() << "Too many files. Only the first file will be handled";
        }
        QString filePath = mimeData->urls().at(0).path();

        gcodeHandler->loadGcode(filePath.toStdString());
        gcodeHandler->drawMesh(scene);
//        this->fitInView(scene->itemsBoundingRect(), Qt::KeepAspectRatio);
    } else qDebug() << "ERROR: Null file url";
}

//void MyGraphicsView::setImage(QImage *image) {
//    xImageSize = image->height();
//    yImageSize = image->width();
//
//    scene->removeItem(pixmap);
//    QImage scaledImage = image->scaled(600, 400, Qt::KeepAspectRatio);
//    pixmap = scene->addPixmap(scaledImage);
//
//    pixmap->setPos(0, 0);
//}

void MyGraphicsView::setXShift(int shift) {
    xImageShift = shift;
}

void MyGraphicsView::setYShift(int shift) {
    yImageShift = shift;
}

int MyGraphicsView::getXShift() {
    return xImageShift;
}

int MyGraphicsView::getYShift() {
    return yImageShift;
}

int MyGraphicsView::getXSize() {
    return xImageSize;
}

int MyGraphicsView::getYSize() {
    return yImageSize;
}
