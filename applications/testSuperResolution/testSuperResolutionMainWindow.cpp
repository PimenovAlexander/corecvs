#include <QtGui/QWheelEvent>
#include <QInputDialog>
#include <QMessageBox>


#include "testSuperResolutionMainWindow.h"
#include "advancedImageWidget.h"
#include "../../utils/fileformats/qtFileLoader.h"
#include "../../utils/corestructs/g12Image.h"
#include "../../core/buffers/rgb24/abstractPainter.h"
#include "resamples.h"
#include "convolution.h"
#include "transformations.h"
#include "modelingProcess.h"
#include <iostream>
TestSuperResolutionMainWindow::TestSuperResolutionMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::TestSuperResolutionMainWindowClass)
    , mImageWidget(NULL)
    , mImage(NULL)
    , mMask(NULL)
{
    mUi->setupUi(this);

    mImageWidget = new AdvancedImageWidget(this);
    setCentralWidget(mImageWidget);
    mImageWidget->addPointTool(0, QString("Select point"), QIcon(":/new/prefix1/lightning.png"));
    showMaximized();
    setWindowTitle("TestSuperResolution");
    connectActions();
    canDelete = true;
}


void TestSuperResolutionMainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}


void TestSuperResolutionMainWindow::connectActions()
{
    connect(mUi->actionOpen, SIGNAL(triggered()), this, SLOT(loadImage()));
    connect(mImageWidget,    SIGNAL(newPointSelected(int, QPoint)), this, SLOT(pointSelected(int, QPoint)));

    connect(mUi->actionShowMask,  SIGNAL(toggled(bool)), this, SLOT(toggleMask()));
    connect(mUi->actionResetMask, SIGNAL(triggered(bool)), this, SLOT(resetMask()));
    connect(mUi->actionUndoMask,  SIGNAL(triggered(bool)), this, SLOT(undoMask()));

    connect(mUi->maskAlphaSpinBox, SIGNAL(valueChanged (int)), this, SLOT(updateViewImage()));
    connect(mUi->maskColorWidget,  SIGNAL(valueChanged()), this, SLOT(updateViewImage()));
    connect(mUi->showEdgeCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateViewImage()));

    connect(mUi -> actionAdd_from_the_File_System, SIGNAL(triggered()), this, SLOT(addElementToCollection()));
    connect(mUi -> actionAdd_from_the_Screen, SIGNAL(triggered()), this, SLOT(addImageFromTheScreenToCollection()));

    connect(mUi -> mWidgetList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_listWidget_itemDoubleClicked(QListWidgetItem*)));

    connect(mUi -> actionClear, SIGNAL(triggered()), this, SLOT(ClearCollection()));//NB!

    connect(mUi -> actionResample_with_bilinear_interpolation, SIGNAL(triggered()), this, SLOT(resampleUsingBilinearInterpolation()));
    connect(mUi -> actionResample_with_Lancsoz_filter_2x2, SIGNAL(triggered()), this, SLOT(resampleUsingLancsozFilter2x2()));
    connect(mUi -> actionResample_with_Lancsoz_filter_4x4, SIGNAL(triggered()), this, SLOT(resampleUsingLancsozFilter4x4()));
    connect(mUi -> actionResample_with_Lancsoz_filter_6x6, SIGNAL(triggered()), this, SLOT(resampleUsingLancsozFilter6x6()));
    connect(mUi -> actionResample_with_Lancsoz_filter_8x8, SIGNAL(triggered()), this, SLOT(resampleUsingLancsozFilter8x8()));
    connect(mUi -> actionResample_with_Nearest_Neighbour, SIGNAL(triggered()), this, SLOT(resampleUsingNearestNeighbour()));
    connect(mUi -> actionSquare_based_Resampling, SIGNAL(triggered()), this, SLOT(resampleUsingSquares()));
    connect(mUi -> actionSimple_modeling_process, SIGNAL(triggered()), this, SLOT(simpleMethodModelingProcess()));

    connect(mUi -> actionCut, SIGNAL(triggered()), this, SLOT(cutImage()));

    connect(mUi -> actionUse_convolution, SIGNAL(triggered()), this, SLOT(convolutionImage()));

    connect(mUi -> actionRotate, SIGNAL(triggered()), this, SLOT(rotateByAngle()));
}


void TestSuperResolutionMainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_X)
    {
        close();
    }

    event->ignore();
}

void TestSuperResolutionMainWindow::loadImage(void)
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Text (*.bmp *.jpg *.png *.gif)"
    );
    QImage *qImage = new QImage(filename);
    if (qImage == NULL) {
        return;
    }

    if (canDelete)
        delete_safe(mImage);
    delete_safe(mMask);
    mImage = QTFileLoader::RGB24BufferFromQImage(qImage);

    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

    delete_safe(qImage);
    updateViewImage();
    canDelete = true;
}


void TestSuperResolutionMainWindow::addImageFromTheScreenToCollection()
{
    if (mImage != NULL)
    {
        bool ok;
        QString name = QInputDialog::getText(this, tr("Enter Name"),tr("Image name:"), QLineEdit::Normal,"", &ok);
        if (ok)
        {
            canDelete = false;
            mImageColection.push_back(mImage);
            QListWidgetItem *item = new QListWidgetItem(name,mUi -> mWidgetList,0);
            item -> setData(Qt::UserRole, QVariant(QString::number(mImageColection.size()-1)));
        }
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Nothing to add");
        msgBox.exec();
    }
}

void TestSuperResolutionMainWindow::toggleMask(void)
{
    updateViewImage();
}

void TestSuperResolutionMainWindow::resetMask(void)
{
    mUndoList.push_back(mMask);
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    mMask = new G8Buffer(mImage->getSize());
    updateViewImage();
}

void TestSuperResolutionMainWindow::undoMask(void)
{
    if (mUndoList.empty()) {
        return;
    }
    delete_safe(mMask);
    mMask = mUndoList.back();
    mUndoList.pop_back();
    updateViewImage();
}


vector<G8Buffer *> mUndoList;

void TestSuperResolutionMainWindow::updateViewImage(void)
{
    RGB24Buffer *toDraw = new RGB24Buffer(mImage);
    RGBColor maskColor = mUi->maskColorWidget->getColor();
    double alpha = (mUi->maskAlphaSpinBox->value()) / 100.0;
    if (mUi->actionShowMask->isChecked())
    {
        for (int i = 0; i < toDraw->h; i++)
        {
            for (int j = 0; j < toDraw->w; j++)
            {
                bool hasmask = false;
                bool hasnomask = false;
                /* so far no optimization here */
                if (mUi->showEdgeCheckBox->isChecked()) {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        for (int dy = -1; dy <= 1; dy++)
                        {
                            if (!mMask->isValidCoord(i + dy, j + dx))
                                continue;
                            if (mMask->element(i + dy, j + dx)) {
                                hasmask = true;
                            } else {
                                hasnomask = true;
                            }
                        }
                    }
                }

                if (mMask->element(i,j)) {
                    if (hasmask && hasnomask) {
                        toDraw->element(i,j) = maskColor;
                    } else {
                        toDraw->element(i,j).r() += (maskColor.r() - toDraw->element(i,j).r()) * alpha;
                        toDraw->element(i,j).g() += (maskColor.g() - toDraw->element(i,j).g()) * alpha;
                        toDraw->element(i,j).b() += (maskColor.b() - toDraw->element(i,j).b()) * alpha;
                    }
                }
            }
        }
    }

    QImage *qImage = new RGB24Image(toDraw);
    mImageWidget->setImage(QSharedPointer<QImage>(qImage));
    delete_safe(toDraw);
}


void TestSuperResolutionMainWindow::maskHue(int hue1, int hue2)
{
    for (int i = 0; i < mImage->h; i++)
    {
       for (int j = 0; j < mImage->w; j++)
       {
           bool result = false;
           int hue = mImage->element(i,j).hue();
           if (hue1 <= hue2)
           {
               if (hue > hue1 && hue < hue2 )
                   result = true;
           } else {
               if (hue < hue2 || hue > hue1 )
                   result = true;
           }
           mMask->element(i,j) = result;

       }
    }
}


class TolerancePredicate {
private:
    G8Buffer *mMask;
    RGBColor  mStartColor;
    int       mTolerance;
public:
    TolerancePredicate (G8Buffer *mask, RGBColor startColor,int tolerance) :
        mMask(mask), mStartColor(startColor), mTolerance(tolerance) {};

    bool operator()(RGB24Buffer *buffer, int x, int y) {
        if (mMask->element(y,x) == 255)
            return false;

        RGBColor currentColor = buffer->element(y,x);
        int r = (int)currentColor.r() - (int)mStartColor.r();
        int g = (int)currentColor.g() - (int)mStartColor.g();
        int b = (int)currentColor.b() - (int)mStartColor.b();

        int sum = abs(r) + abs(g) + abs(b);
        if (sum > mTolerance)
           return false;
        return true;
     }

     void mark(RGB24Buffer *buffer, int x, int y) {
         mMask->element(y,x) = 255;
     }

};

void TestSuperResolutionMainWindow::recursiveTolerance(RGBColor startColor, int tolerance, int x, int y)
{
    if (!mMask->isValidCoord(y,x))
        return;
    if (mMask->element(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);
    int r = (int)currentColor.r() - (int)startColor.r();
    int g = (int)currentColor.g() - (int)startColor.g();
    int b = (int)currentColor.b() - (int)startColor.b();

    int sum = abs(r) + abs(g) + abs(b);
    if (sum > tolerance)
        return;

    mMask->element(y,x) = 255;

    recursiveTolerance(startColor, tolerance, x - 1, y    );
    recursiveTolerance(startColor, tolerance, x + 1, y    );
    recursiveTolerance(startColor, tolerance, x    , y + 1);
    recursiveTolerance(startColor, tolerance, x    , y - 1);
}

void TestSuperResolutionMainWindow::maskTolerance(QPoint point)
{
    int x = point.x();
    int y = point.y();

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    recursiveTolerance(currentColor, mUi->toleranceSpinBox->value(), x, y);
}

void TestSuperResolutionMainWindow::maskTolerance1(QPoint point)
{
    int x = point.x();
    int y = point.y();

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    TolerancePredicate predicate(mMask, currentColor, mUi->toleranceSpinBox->value());
    AbstractPainter<RGB24Buffer> painter(mImage);
    painter.floodFill(x,y, predicate);
}

void TestSuperResolutionMainWindow::pointSelected(int toolID, QPoint point)
{
    qDebug() << "Point Selected" << point;
    mUndoList.push_back(new G8Buffer(mMask));
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    switch (mUi->maskComboBox->currentIndex())
    {
        case 0:
            maskHue(mUi->hue1SpinBox->value(), mUi->hue2SpinBox->value());
            break;
        case 1:
            maskTolerance(point);
            break;
        case 2:
            maskTolerance1(point);
            break;
    }
    updateViewImage();
}

TestSuperResolutionMainWindow::~TestSuperResolutionMainWindow()
{
    delete_safe(mImageWidget);
    delete_safe(mUi);
}


void TestSuperResolutionMainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QVariant filename = item -> data(Qt::UserRole);
    int i = filename.toInt();
    if (canDelete)
        delete_safe(mImage);
    delete_safe(mMask);
    canDelete = false;
    mImage = mImageColection.at(i);
    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);
    updateViewImage();
}


void TestSuperResolutionMainWindow::addElementToCollection() {
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Text (*.bmp *.jpg *.png *.gif)"
    );
    QImage *qImage = new QImage(filename);
    if (qImage == NULL) {
        return;
    }

    QString name = filename.section('/', -1);
    RGB24Buffer *newImage = QTFileLoader::RGB24BufferFromQImage(qImage);
    mImageColection.push_back(newImage);
    QListWidgetItem *item = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item -> setData(Qt::UserRole, QVariant(QString::number(mImageColection.size()-1)));
}
void TestSuperResolutionMainWindow::ClearCollection() {

    mUi -> mWidgetList -> clear();

}

void TestSuperResolutionMainWindow::cutImage() { //cuts image and creates collection of 4 parts of image
    int pxls = mUi->pixelsSpinBox->value();
    int count = mUi ->countImSpinBox->value()-1;
    QString filename = "1.bmp";
    QString name = filename.section('/', -1);

    if (mImage == NULL) {
        return;
    }

    if (mImage->w <= pxls*count){
        QMessageBox msgBox;
        msgBox.setText ("The size of the image is too small.");
        msgBox.exec ();
        return;
    }

    ClearCollection();

    RGB24Buffer *toDraw = new RGB24Buffer(mImage);
    toDraw ->w = toDraw ->w - pxls*count;
    QImage *qImage = new RGB24Image(toDraw);
    qImage->save(filename);

    QListWidgetItem *item = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item -> setData(Qt::UserRole, QVariant(filename));

    for (int k = 1; k <= count; k++) {
        *toDraw = mImage;
        for (int i = 0; i < mImage->h; i++)
            {
               for (int j = 0; j < mImage->w - pxls * count ; j++)
               {
                   toDraw->element(i,j) = mImage -> element(i,j + pxls * k);
               }

        }
        toDraw ->w = toDraw ->w - pxls*count;
        *qImage = RGB24Image(toDraw);

        filename = QString::number(k+1);
        filename += ".bmp";
        qImage->save(filename);
        name = filename.section('/', -1);
        QListWidgetItem *item = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
        item -> setData(Qt::UserRole, QVariant(filename));
    }
}


void TestSuperResolutionMainWindow::resampleUsingBilinearInterpolation() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithBilinearInterpolation(mImage,newSize);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}


void TestSuperResolutionMainWindow::resampleUsingLancsozFilter2x2() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithLancsozFilter(mImage,newSize,1);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}

void TestSuperResolutionMainWindow::resampleUsingLancsozFilter4x4() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithLancsozFilter(mImage,newSize,2);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}

void TestSuperResolutionMainWindow::resampleUsingLancsozFilter6x6() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithLancsozFilter(mImage,newSize,3);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}

void TestSuperResolutionMainWindow::resampleUsingLancsozFilter8x8() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithLancsozFilter(mImage,newSize,4);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}


void TestSuperResolutionMainWindow::resampleUsingNearestNeighbour() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok)
    {
        RGB24Buffer *result = resampleWithNearestNeighbour(mImage,newSize);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}

void TestSuperResolutionMainWindow::convolutionImage() {

        RGB24Buffer *result = convolution(mImage);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();

}


void TestSuperResolutionMainWindow::rotateByAngle() {
    bool ok;
    double angle = QInputDialog::getDouble(
                this,
                "Print the angle (degree)",
                tr("Angle:"),
                0,
                -1000,
                1000,
                2,
                &ok
                );
    if (ok) {
        RGB24Buffer *result = rotate(mImage,angle);
        if (canDelete)
            delete_safe(mImage);
        delete_safe(mMask);
        mImage = result;
        mMask = new G8Buffer(mImage->getSize());
        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

        updateViewImage();
    }
}

void TestSuperResolutionMainWindow::resampleUsingSquares() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                5,
                2,
                &ok);
    if (ok) {
        double shiftX = QInputDialog::getDouble(
                    this,
                    "Print the X-shift (in pixels)",
                    tr("X-shift:"),
                    0,
                    -1000,
                    1000,
                    2,
                    &ok
                    );
        if (ok) {
                    double shiftY = QInputDialog::getDouble(
                                this,
                                "Print the Y-shift (in pixels)",
                                tr("Y-shift:"),
                                0,
                                -1000,
                                1000,
                                2,
                                &ok
                                );
                    if (ok) {
                        double angle = QInputDialog::getDouble(
                                    this,
                                    "Print the angle (degree)",
                                    tr("Angle:"),
                                    0,
                                    -1000,
                                    1000,
                                    2,
                                    &ok
                                    );
                        if (ok) {
                            RGB24Buffer *result = squareBasedResampling(mImage,newSize,shiftX,shiftY,angle);
                            if (canDelete)
                                delete_safe(mImage);
                            delete_safe(mMask);
                            mImage = result;
                            mMask = new G8Buffer(mImage->getSize());
                            AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

                            updateViewImage();
                        }
                    }
        }

    }
}


void TestSuperResolutionMainWindow::simpleMethodModelingProcess() {
    bool ok;
    double newSize = QInputDialog::getDouble(
                this,
                "Print the compression ratio",
                tr("Ratio:"),
                1,
                0.01,
                1,
                2,
                &ok);
    if (ok) {
        int shiftX = QInputDialog::getInt(
                    this,
                    "Print the X-shift (in pixels)",
                    tr("X-shift:"),
                    0,
                    0,
                    1000,
                    1,
                    &ok
                    );
        if (ok) {
                    int shiftY = QInputDialog::getInt(
                                this,
                                "Print the Y-shift (in pixels)",
                                tr("Y-shift:"),
                                0,
                                0,
                                1000,
                                1,
                                &ok
                                );
                    if (ok) {
                        RGB24Buffer *result = simpleModelingProcess(mImage,newSize,shiftX,shiftY);
                        delete_safe(mImage);
                        delete_safe(mMask);
                        mImage = result;
                        mMask = new G8Buffer(mImage->getSize());
                        AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

                        updateViewImage();
                    }
        }

    }
}