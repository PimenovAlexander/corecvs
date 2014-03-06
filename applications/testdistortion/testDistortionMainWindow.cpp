#include <QtGui/QWheelEvent>

#include "testbedMainWindow.h"
#include "advancedImageWidget.h"
#include "../../utils/fileformats/qtFileLoader.h"
#include "../../utils/corestructs/g12Image.h"
#include "../../core/buffers/rgb24/abstractPainter.h"


TestbedMainWindow::TestbedMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::TestbedMainWindowClass)
    , mImageWidget(NULL)
    , mDistrtionWidget(NULL)
    , mImage(NULL)
    , mMask(NULL)
{
    mUi->setupUi(this);

    isEdgeFilterActive = false;

    mImageWidget = new AdvancedImageWidget(this);
    setCentralWidget(mImageWidget);

    mDistrtionWidget = new DistortionWidget();

    mImageWidget->addPointTool(0, QString("Select point"), QIcon(":/new/prefix1/lightning.png"));
    showMaximized();
    setWindowTitle("Testbed");
    connectActions();
}


void TestbedMainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}


void TestbedMainWindow::connectActions()
{
    connect(mUi->actionOpen, SIGNAL(triggered()), this, SLOT(loadImage()));
    connect(mImageWidget,    SIGNAL(newPointSelected(int, QPoint)), this, SLOT(pointSelected(int, QPoint)));

    connect(mUi->actionShowMask,  SIGNAL(toggled(bool)), this, SLOT(toggleMask()));
    connect(mUi->actionResetMask, SIGNAL(triggered(bool)), this, SLOT(resetMask()));
    connect(mUi->actionUndoMask,  SIGNAL(triggered(bool)), this, SLOT(undoMask()));

    connect(mUi->maskAlphaSpinBox, SIGNAL(valueChanged (int)), this, SLOT(updateViewImage()));
    connect(mUi->maskColorWidget,  SIGNAL(valueChanged()), this, SLOT(updateViewImage()));
    connect(mUi->showEdgeCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateViewImage()));

    connect(mUi->actionDistortionCorrection, SIGNAL(triggered(bool)), this, SLOT(openDistortionWindow()));

    connect(mUi->actionToggleEdgeFilter, SIGNAL(toggled(bool)), this, SLOT(toogleEdgeFilter()));
}


void TestbedMainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->modifiers() == Qt::AltModifier && event->key() == Qt::Key_X)
    {
        close();
    }

    event->ignore();
}

void TestbedMainWindow::loadImage(void)
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Text (*.bmp *.jpg *.png *.gif)"
    );
    QImage *qImage = new QImage(filename);
    if (qImage == NULL)
    {
        return;
    }
    delete_safe(mImage);
    delete_safe(mMask);
    mImage = QTFileLoader::RGB24BufferFromQImage(qImage);
    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

    delete_safe(qImage);
    updateImages();
    updateViewImage();
}

void TestbedMainWindow::toggleMask(void)
{
    updateViewImage();
}

void TestbedMainWindow::resetMask(void)
{
    mUndoList.push_back(mMask);
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    mMask = new G8Buffer(mImage->getSize());
    updateViewImage();
}

void TestbedMainWindow::updateImages(void)
{
    originalImage = new RGB24Buffer(mImage);

    G12Buffer *originalGImage = originalImage->toG12Buffer();

    int w =originalGImage->getW();
    int h =originalGImage->getH();
    edgeFilterImage = new G12Buffer(h,w);

    int verArray[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};

    for (int i = 1; i < h-1; i++)
    {
        for (int j = 1; j < w-1; j++)
        {
            int verSum = 0;
            int gorSum = 0;
            for (int dx = -1, l = 0; dx <= 1; dx ++, l++)
            {
                for (int dy = -1, k = 0; dy <= 1; dy ++, k++)
                {
                    verSum += originalGImage->element(i + dy, j + dx) * verArray[l][k];
                    gorSum += originalGImage->element(i + dy, j + dx) * verArray[k][l];
                }
            }
            edgeFilterImage->element(i,j) = sqrt((verSum*verSum)+(gorSum*gorSum));
        }
    }
}

G12Buffer* executeEdgeFilter(RGB24Buffer *image)
{
    G12Buffer *buffer = image->toG12Buffer();

    int w =buffer->getW();
    int h =buffer->getH();

    G12Buffer *newBuffer = new G12Buffer(h,w);

    int verArray[3][3] = {{1,0,-1},{2,0,-2},{1,0,-1}};

    for (int i = 1; i < h-1; i++)
    {
        for (int j = 1; j < w-1; j++)
        {
            int verSum = 0;
            int gorSum = 0;
            for (int dx = -1, l = 0; dx <= 1; dx ++, l++)
            {
                for (int dy = -1, k = 0; dy <= 1; dy ++, k++)
                {
                    verSum += buffer->element(i + dy, j + dx) * verArray[l][k];
                    gorSum += buffer->element(i + dy, j + dx) * verArray[k][l];
                }
            }
            int value = sqrt((verSum*verSum)+(gorSum*gorSum));
            newBuffer->element(i,j) = value*value*value;
        }
    }
    return newBuffer;
}

void TestbedMainWindow::toogleEdgeFilter(void)
{
    isEdgeFilterActive=!isEdgeFilterActive;

    if (mImage==NULL)
    {
        return;
    }

    if (isEdgeFilterActive)
    {
        mImage=new RGB24Buffer(edgeFilterImage);
    }
    else
    {
        mImage=new RGB24Buffer(originalImage);
    }
    updateViewImage();

}

void TestbedMainWindow::openDistortionWindow(void)
{
    mDistrtionWidget->show();
    mDistrtionWidget->raise();

    if (mImage != NULL)
    {
        mDistrtionWidget->setBuffer(mImage->toG12Buffer());
    }
}

void TestbedMainWindow::undoMask(void)
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

void TestbedMainWindow::updateViewImage(void)
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
                bool hasMask = false;
                bool hasNoMask = false;
                /* so far no optimization here */
                if (mUi->showEdgeCheckBox->isChecked()) {
                    for (int dx = -1; dx <= 1; dx++)
                    {
                        for (int dy = -1; dy <= 1; dy++)
                        {
                            if (!mMask->isValidCoord(i + dy, j + dx))
                                continue;
                            if (mMask->element(i + dy, j + dx)) {
                                hasMask = true;
                            } else {
                                hasNoMask = true;
                            }
                        }
                    }
                }

                if (mMask->element(i,j)) {
                    if (hasMask && hasNoMask) {
                        toDraw->element(i, j) = maskColor;
                    } else {
                        toDraw->element(i, j).r() += (maskColor.r() - toDraw->element(i,j).r()) * alpha;
                        toDraw->element(i, j).g() += (maskColor.g() - toDraw->element(i,j).g()) * alpha;
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


void TestbedMainWindow::maskHue(int hue1, int hue2)
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

void TestbedMainWindow::recursiveTolerance(RGBColor startColor, int tolerance, int x, int y)
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

void TestbedMainWindow::maskTolerance(QPoint point)
{
    int x = point.x();
    int y = point.y();

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    recursiveTolerance(currentColor, mUi->toleranceSpinBox->value(), x, y);
}

void TestbedMainWindow::maskTolerance1(QPoint point)
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

void TestbedMainWindow::pointSelected(int toolID, QPoint point)
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

TestbedMainWindow::~TestbedMainWindow()
{
    delete_safe(mImageWidget);
    delete_safe(mUi);
}
