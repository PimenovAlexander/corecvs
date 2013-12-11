#include <QtGui/QWheelEvent>

#include "testSuperResolutionMainWindow.h"
#include "advancedImageWidget.h"
#include "../../utils/fileformats/qtFileLoader.h"
#include "../../utils/corestructs/g12Image.h"
#include "../../core/buffers/rgb24/abstractPainter.h"
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

    connect(mUi -> actionAdd, SIGNAL(triggered()), this, SLOT(addElementToCollection()));
    connect(mUi -> mWidgetList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(on_listWidget_itemDoubleClicked(QListWidgetItem*)));
    connect(mUi -> actionClear, SIGNAL(triggered()), this, SLOT(ClearCollection()));
    connect(mUi -> actionCut, SIGNAL(triggered()), this, SLOT(cutImage()));
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
    delete_safe(mImage);
    delete_safe(mMask);
    mImage = QTFileLoader::RGB24BufferFromQImage(qImage);
    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

    delete_safe(qImage);
    updateViewImage();
}


void TestSuperResolutionMainWindow::addImageToCollection() { }

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
    QImage *qImage = new QImage(filename.toString());
    delete_safe(mImage);
    delete_safe(mMask);
    mImage = QTFileLoader::RGB24BufferFromQImage(qImage);
    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

    delete_safe(qImage);
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

    QListWidgetItem *item = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item -> setData(Qt::UserRole, QVariant(filename));
}
void TestSuperResolutionMainWindow::ClearCollection() {

    mUi -> mWidgetList -> clear();
    /*QString filename = QFileDialog::getOpenFileName(
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

    QListWidgetItem *item = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item -> setData(Qt::UserRole, QVariant(filename));*/
}

void TestSuperResolutionMainWindow::cutImage() { //cuts image and creates collection of 4 parts of image
    int pxls = mUi->pixelsSpinBox->value();
    QString filename = "1.bmp";
    QString name = filename.section('/', -1);

    if (mImage == NULL || mImage->h < pxls || mImage->w < pxls) {
        return;
    }

    ClearCollection();

    RGB24Buffer *toDraw = new RGB24Buffer(mImage);
    toDraw ->h = toDraw ->h - pxls;
    QImage *qImage = new RGB24Image(toDraw);
    qImage->save("1.bmp");

    QListWidgetItem *item1 = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item1 -> setData(Qt::UserRole, QVariant(filename));

    *toDraw = mImage;
    toDraw ->w = toDraw ->w - pxls;
    *qImage = RGB24Image(toDraw);
    qImage->save("2.bmp");

    filename = "2.bmp";
    name = filename.section('/', -1);
    QListWidgetItem *item2 = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item2 -> setData(Qt::UserRole, QVariant(filename));

    *toDraw = mImage;
    for (int i = 0; i < mImage->h-pxls; i++)
        {
           for (int j = 0; j < mImage->w; j++)
           {
               toDraw->element(i,j) = mImage -> element(i+pxls,j);
           }

    }
    toDraw ->h = toDraw ->h - pxls;
    *qImage = RGB24Image(toDraw);
    qImage->save("3.bmp");

    filename = "3.bmp";
    name = filename.section('/', -1);
    QListWidgetItem *item3 = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item3 -> setData(Qt::UserRole, QVariant(filename));

    *toDraw = mImage;
    for (int i = 0; i < mImage->h; i++)
        {
           for (int j = 0; j < mImage->w-pxls; j++)
           {
               toDraw->element(i,j) = mImage -> element(i,j+pxls);
           }

    }
    toDraw ->w = toDraw ->w - pxls;
    *qImage = RGB24Image(toDraw);
    qImage->save("4.bmp");

    filename = "4.bmp";
    name = filename.section('/', -1);
    QListWidgetItem *item4 = new QListWidgetItem(QIcon(filename),name,mUi -> mWidgetList,0);
    item4 -> setData(Qt::UserRole, QVariant(filename));
}



