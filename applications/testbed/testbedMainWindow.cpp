#include <QtGui/QWheelEvent>

#include "testbedMainWindow.h"
#include "advancedImageWidget.h"
#include "qtFileLoader.h"
#include "g12Image.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/morphological/morphological.h"
#include "core/fileformats/bmpLoader.h"
#include "core/segmentation/segmentator.h"
#include "pointScene.h"
#include "localHistogram.h"
#include "core/filters/cannyFilter.h"
#include "core/utils/preciseTimer.h"
#include "widgets/testbedImageWidget.h"
#include "gapFiller.h"
//#include "core/geometry/rectangle.h"

using corecvs::Segmentator;
//using corecvs::Rectangle;

enum ToolId {
    MASK_TOOL,
    EREASE_TOOL,
    HIST_INFO1,
    HIST_INFO2,
    UNICOLOR_SELECTOR,
    TEXTURE_SELECTOR
};


TestbedMainWindow::TestbedMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::TestbedMainWindowClass)
    , mImageWidget(NULL)
    , mImage(NULL)
    , mMask(NULL)
    , mMaskBlended(NULL)
    , mMaskStore(NULL)
    , mHComp(NULL)
    , mSComp(NULL)
    , mVComp(NULL)
    , mPrincipal(NULL)
    , mPrincipal2(NULL)
    , mPrincipal3(NULL)
    , mHistBuffer(NULL)
    , mHist2DBuffer(NULL)
    , mLocalHistWidget(NULL)
    , mLocalHist2DWidget(NULL)
    , mCannyEdges(NULL)
    , mEdges(NULL)
{
    mUi->setupUi(this);

    Log::mLogDrains.add(mUi->loggingWidget);

    mImageWidget = new TestbedImageWidget(this);
    setCentralWidget(mImageWidget);



	mImageWidget->addPointTool    (MASK_TOOL, QString("Select point"), QIcon(":/new/prefix1/lightning.png"));
	mImageWidget->addSelectionTool(0, QString("Select rect") , QIcon(":/new/prefix1/select_reg.png"));

	mImageWidget->addPointTool    (EREASE_TOOL, QString("Ereaser"), QIcon(":/new/prefix1/draw_eraser.png"));
    mImageWidget->addPointTool    (HIST_INFO1, QString("Histogram"), QIcon(":/new/prefix1/chart_bar.png"));
    mImageWidget->addPointTool    (HIST_INFO2, QString("Histogram2D"), QIcon(":/new/prefix1/workspace.png"));

    mImageWidget->addPointTool    (UNICOLOR_SELECTOR, QString("Unicolor selector"), QIcon(":/icons/color_picker_alternative.png"));
    mImageWidget->addPointTool    (TEXTURE_SELECTOR, QString("Textured selector"), QIcon(":/icons/color_management.png"));

    mLocalHistWidget   = new AdvancedImageWidget();
    mLocalHist2DWidget = new AdvancedImageWidget();
    m3DHist = new CloudViewDialog();
    mGraphPlot = new GraphPlotDialog();
    //m3DHist->show();

	showMaximized();
    setWindowTitle("Testbed");
    connectActions();
    mUi->actionShowMask->setChecked(true);



    /* Some other interface related fixes */
    connect(mUi->fingerSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(radiusChanged()));
    mUi->maskColorWidget->setParameters(RgbColorParameters(40, 250, 40));

    L_INFO_P("Creating the widget");
}

void TestbedMainWindow::radiusChanged()
{
    mImageWidget->decorationSize = (double)mUi->fingerSizeSpinBox->value() * (12.8 / 4 * 3);
}


void TestbedMainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    qApp->quit();
}


void TestbedMainWindow::connectActions()
{
    connect(mUi->actionOpen,      SIGNAL(triggered()), this, SLOT(loadImage()));
    connect(mUi->actionReprocess, SIGNAL(triggered()), this, SLOT(preprocessImage()));

    connect(mImageWidget,    SIGNAL(newPointSelected(int, QPoint)), this, SLOT(pointSelected(int, QPoint)));
    connect(mImageWidget,    SIGNAL(pointToolMoved(int, QPoint)), this, SLOT(pointSelectedMoved(int, QPoint)));
    connect(mImageWidget,    SIGNAL(newAreaSelected (int, QRect )), this, SLOT(rectSelected (int, QRect )));

    connect(mUi->actionShowMask,  SIGNAL(toggled(bool)),   this, SLOT(toggleMask()));
    connect(mUi->actionResetMask, SIGNAL(triggered(bool)), this, SLOT(resetMask()));
    connect(mUi->actionUndoMask,  SIGNAL(triggered(bool)), this, SLOT(undoMask()));

    connect(mUi->maskAlphaSpinBox,   SIGNAL(valueChanged (int)),       this, SLOT(updateViewImage()));
    connect(mUi->maskColorWidget,    SIGNAL(paramsChanged()),          this, SLOT(updateViewImage()));
    connect(mUi->showEdgeCheckBox,   SIGNAL(toggled(bool)),            this, SLOT(updateViewImage()));
    connect(mUi->blendEdgeCheckBox,  SIGNAL(toggled(bool)),            this, SLOT(updateViewImage()));
    connect(mUi->levelDebugCheckBox, SIGNAL(toggled(bool)),            this, SLOT(updateViewImage()));
    connect(mUi->levelSpinBox,       SIGNAL(valueChanged (int)),       this, SLOT(updateViewImage()));
    connect(mUi->bufferSelectBox,    SIGNAL(currentIndexChanged(int)), this, SLOT(updateViewImage()));

    connect(mUi->eraserSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(paramChanged()));

    connect(mUi->action3DView         , SIGNAL(toggled(bool)), m3DHist           , SLOT(setVisible(bool)));
    connect(mUi->actionThresholdGraph , SIGNAL(toggled(bool)), mGraphPlot        , SLOT(setVisible(bool)));
    connect(mUi->actionHistogram      , SIGNAL(toggled(bool)), mLocalHistWidget  , SLOT(setVisible(bool)));
    connect(mUi->actionHistogram2D    , SIGNAL(toggled(bool)), mLocalHist2DWidget, SLOT(setVisible(bool)));


    connect(mGraphPlot,    SIGNAL(newTimeSelected(int)), mUi->levelSpinBox, SLOT(setValue(int)));

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

    L_INFO_P("Loading <%s>", filename.toLatin1().data());

    QRegExp pattern("_t(\\d*)\\.");
    if (pattern.indexIn(filename) != -1)
    {
        QString toleranceStr = pattern.cap(1);
        qDebug() << "In string"<< pattern.cap(0) <<"Assuming historgam tolerance:" << toleranceStr;

        int tolerance = toleranceStr.toInt();
        mUi->histToleranceSpinBox->setValue(tolerance);
    }

    QRegExp pattern1("_b(\\d*)\\.");
    if (pattern1.indexIn(filename) != -1)
    {
        QString toleranceStr = pattern1.cap(1);
        qDebug() << "In string"<< pattern1.cap(0) <<"Assuming color tolerance:" << toleranceStr;

        int tolerance = toleranceStr.toInt();
        mUi->toleranceSpinBox->setValue(tolerance);
    }


    QImage *qOrigImage = new QImage(filename);
    if (qOrigImage == NULL || qOrigImage->isNull()) {
        L_INFO_P("Failed to load");
        return;
    }
    QImage qImage;

    if (qOrigImage->height() > qOrigImage->width())
    {
        qImage = qOrigImage->scaled(mUi->imageWSpinBox->value(), mUi->imageHSpinBox->value(), Qt::KeepAspectRatio);
    } else {
        qImage = qOrigImage->scaled(mUi->imageHSpinBox->value(), mUi->imageWSpinBox->value(), Qt::KeepAspectRatio);
    }

    delete_safe(qOrigImage);

    delete_safe(mImage);

    mImage = QTFileLoader::RGB24BufferFromQImage(&qImage);

    delete_safe(mMask);
    mMask = new G8Buffer(mImage->getSize());
    AbstractPainter<G8Buffer>(mMask).drawCircle(mImage->w / 2, mImage->h / 2, (!mImage->getSize()) / 4, 255);

    preprocessImage();

}

G8Buffer *TestbedMainWindow::projectToDirection(RGB24Buffer *input, const Vector3dd &direction)
{
    double min =  std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();

    G8Buffer *projected = new G8Buffer(input->getSize(), false);

    /*Find minimum and maximum */
    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            RGBColor &color = input->element(i,j);
            Vector3dd c(color.r(), color.g(), color.b());
            double projection = c & direction;
            if (projection > max) max = projection;
            if (projection < min) min = projection;
        }
    }

    double mean = (min + max) / 2;
    double scale = (max - min);


    for (int i = 0; i < input->h; i++)
    {
        for (int j = 0; j < input->w; j++)
        {
            RGBColor &color = input->element(i,j);
            Vector3dd c(color.r(), color.g(), color.b());
            double projection = (c & direction) - mean;
            int result = (projection / scale * G8Buffer::BUFFER_MAX_VALUE) + (G8Buffer::BUFFER_MAX_VALUE / 2);
            if (result > G8Buffer::BUFFER_MAX_VALUE) result = G8Buffer::BUFFER_MAX_VALUE;
            if (result < 0) result = 0;
            projected->element(i,j) = result;
        }
    }

    return projected;
}


void TestbedMainWindow::preprocessImage(void)
{
    if (mImage == NULL) {
        return;
    }

    L_INFO_P("Starting to preprocess");
    PointScene *scene = new PointScene();
    //scene->scene.push_back(PointScene::Point(Vector3dd(1.0, 1.0, 1.0)));
    scene->showBuffer(mImage);
    m3DHist->setNewScenePointer(QSharedPointer<Scene3D>(scene), CloudViewDialog::MAIN_SCENE);

    Vector3dd mean(0.0);

    for (int i = 0; i < mImage->h; i++)
    {
        for (int j = 0; j < mImage->w; j++)
        {
            mean += mImage->element(i,j).toDouble();
        }
    }
    mean /= (mImage->h * mImage->w);

    EllipticalApproximationUnified<Vector3dd> ellip;
    for (int i = 0; i < mImage->h; i++)
    {
        for (int j = 0; j < mImage->w; j++)
        {
            ellip.addPoint(mImage->element(i,j).toDouble() - mean);
        }
    }
    ellip.getEllipseParameters();
    qDebug() << "Size is: "<< ellip.mAxes.size();

    EllApproxScene *sceneEl = new EllApproxScene(mean, ellip);
    m3DHist->setNewScenePointer(QSharedPointer<Scene3D>(sceneEl), CloudViewDialog::ADDITIONAL_SCENE);
    L_INFO_P("Color distribution analyzed");
    L_INFO_P("Preparing HSV presentation");

    delete_safe(mHComp);
    delete_safe(mSComp);
    delete_safe(mVComp);
    mHComp = new G8Buffer(mImage->getSize(), false);
    mSComp = new G8Buffer(mImage->getSize(), false);
    mVComp = new G8Buffer(mImage->getSize(), false);

    for (int i = 0; i < mImage->h; i++)
    {
        for (int j = 0; j < mImage->w; j++)
        {
            RGBColor &color = mImage->element(i,j);
            mHComp->element(i,j) = color.hue() * 255 / 360;
            mSComp->element(i,j) = color.saturation();
            mVComp->element(i,j) = color.value();
        }
    }
    L_INFO_P("Preparing edges");
    G12Buffer *tempBuffer = G8Buffer::toG12Buffer(mVComp);
    delete_safe(mEdges);

    delete_safe(mCannyEdges);
    delete_safe(mEdges);

    CannyParameters *cannyParams = mUi->cannyParametersWidget->createParameters();

    DerivativeBuffer *derivativeBuffer = NULL;
    mCannyEdges = CannyFilter::doFilter(tempBuffer, *cannyParams, &derivativeBuffer);
    mEdges = derivativeBuffer->gradientMagnitudeBuffer(10.0);

    delete_safe(derivativeBuffer);

    delete_safe(cannyParams);
    delete_safe(tempBuffer);


    L_INFO_P("Preparing projected buffer");
    Vector3dd mainDirection = ellip.mAxes[0];
    mainDirection.normalise();
    L_INFO << "Principal component is:" << mainDirection;
    delete_safe(mPrincipal);
    mPrincipal = projectToDirection(mImage, mainDirection);

    Vector3dd secondaryDirection = ellip.mAxes[1];
    secondaryDirection.normalise();
    L_INFO << "Secondary component is:" << secondaryDirection;
    delete_safe(mPrincipal2);
    mPrincipal2 = projectToDirection(mImage, secondaryDirection);

    Vector3dd thirdDirection = ellip.mAxes[2];
    thirdDirection.normalise();
    L_INFO << "Third component is:" << thirdDirection;
    delete_safe(mPrincipal3);
    mPrincipal3 = projectToDirection(mImage, thirdDirection);

    PreciseTimer timer;
    L_INFO_P("Preparing local histogram buffer");
    timer = PreciseTimer::currentTime();

    delete_safe(mHistBuffer);
    mHistBuffer = new AbstractBuffer<LocalHistogram>(mPrincipal->getSize());
    int bound = mUi->histRadiusSpinBox->value();
    for (int i = bound; i < mPrincipal->h - bound; i++)
    {
        for (int j = bound; j < mPrincipal->w - bound; j++)
        {
            for (int dy = -bound; dy <= bound; dy++)
            {
                for (int dx = -bound; dx <= bound; dx++)
                {
                    mHistBuffer->element(i, j).inc(mPrincipal->element(i + dy, j + dx));
                }
            }

        }
    }
    L_INFO_P("  Done in %d", timer.usecsToNow());


    L_INFO_P("Preparing local histogram2D buffer");
    timer = PreciseTimer::currentTime();

    delete_safe(mHist2DBuffer);
    mHist2DBuffer = new Histogram2DBuffer(mPrincipal->getSize());
    bound = mUi->histRadiusSpinBox->value();
    for (int i = bound; i < mPrincipal->h - bound; i++)
    {
        for (int j = bound; j < mPrincipal->w - bound; j++)
        {
            LocalHistogram2D *hist = &mHist2DBuffer->element(i, j);
            hist->isSet = true;
            for (int dy = -bound; dy <= bound; dy++)
            {
                for (int dx = -bound; dx <= bound; dx++)
                {
                    hist->inc(mPrincipal->element(i + dy, j + dx), mPrincipal2->element(i + dy, j + dx));
                }
            }

        }
    }
    L_INFO_P("  Done in %d", timer.usecsToNow());


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

void TestbedMainWindow::prepareBlendedMask()
{
    delete_safe(mMaskBlended);
    mMaskBlended = new G8Buffer(mMask->getSize());

    const int m = 4;
    int size = 3;
    G12Buffer *core = new G12Buffer(2*size + 1, 2*size + 1);
    AbstractPainter<G12Buffer>(core).drawCircle(size, size, size, 0xFFF);
    BMPLoader().save("eandd_kernel.bmp", core);

    G8Buffer *mMaskBase = Morphological::dilate (mMask, core, size, size);


    for (int i = 0; i < mMaskBlended->h; i++)
    {
        for (int j = 0; j < mMaskBlended->w; j++)
        {
            int sum = 0;
            int count = 0;
            for (int dy = -m; dy <= m; dy ++)
            {
                for (int dx = -m; dx <= m; dx ++)
                {
                    int x = j + dx;
                    int y = i + dy;

                    if (!mMask->isValidCoord(y,x))
                        continue;

                    count++;
                    if (mMask->element(y,x))
                        sum++;
                }
            }

            int result = sum * 255 / count;
            mMaskBlended->element(i,j) = result;
        }
    }

    delete_safe(mMaskBase);
}


void TestbedMainWindow::updateViewImage(void)
{
    enum {IMAGE, MASK, HUE, SATURATION, VALUE, EDGES, CANNY, PRINCIPAL, SECONDARY, THIRD};

    switch (mUi->bufferSelectBox->currentIndex()) {
        case IMAGE:
            {
                if (mImage == NULL) {
                    return;
                }

                if (mUi->blendEdgeCheckBox->isChecked())
                {
                    prepareBlendedMask();
                }

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

                            if (mUi->blendEdgeCheckBox->isChecked())
                            {
                                double scaler = alpha * mMaskBlended->element(i,j) / 255;
                                toDraw->element(i,j).r() += (maskColor.r() - toDraw->element(i,j).r()) * scaler;
                                toDraw->element(i,j).g() += (maskColor.g() - toDraw->element(i,j).g()) * scaler;
                                toDraw->element(i,j).b() += (maskColor.b() - toDraw->element(i,j).b()) * scaler;
                            } else {
                                if (mMask->element(i,j)) {
                                    toDraw->element(i,j).r() += (maskColor.r() - toDraw->element(i,j).r()) * alpha;
                                    toDraw->element(i,j).g() += (maskColor.g() - toDraw->element(i,j).g()) * alpha;
                                    toDraw->element(i,j).b() += (maskColor.b() - toDraw->element(i,j).b()) * alpha;
                                }
                            }


                            if (mMask->element(i,j)) {
                                if (hasmask && hasnomask) {
                                    toDraw->element(i,j) = mUi->edgeColorWidget->getColor();
                                }
                            }

                            if (mUi->levelDebugCheckBox->isChecked() && mMaskStore != NULL && mMaskStore->element(i,j) != 0)
                            {
                                if (mMaskStore->element(i,j) > mUi->levelSpinBox->value())
                                {
                                    toDraw->element(i,j) = RGBColor::Red();
                                } else {
                                    toDraw->element(i,j) = RGBColor::Blue();
                                }

                            }
                        }
                    }
                }

                QImage *qImage = new RGB24Image(toDraw);
                mImageWidget->setImage(QSharedPointer<QImage>(qImage));
                delete_safe(toDraw);
            }
            break;
        case MASK:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mMask)));
            break;
        case HUE:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mHComp)));
            break;
        case SATURATION:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mSComp)));
            break;
        case VALUE:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mVComp)));
            break;
        case EDGES:
                mImageWidget->setImage(QSharedPointer<QImage>(new G12Image(mEdges)));
            break;
        case CANNY:
                mImageWidget->setImage(QSharedPointer<QImage>(new G12Image(mCannyEdges)));
            break;
        case PRINCIPAL:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mPrincipal)));
            break;
        case SECONDARY:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mPrincipal2)));
            break;
        case THIRD:
        default:
                mImageWidget->setImage(QSharedPointer<QImage>(new G8Image(mPrincipal3)));
            break;
    }


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
        if (sum > mTolerance) {
           return false;
        }
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
    L_INFO_P("Starting the fill");

    if (!mMask->isValidCoord(y,x)) {
        return;
    }
    RGBColor currentColor = mImage->element(y,x);

    recursiveTolerance(currentColor, mUi->toleranceSpinBox->value(), x, y);
    L_INFO_P("Fill finished");
}

void TestbedMainWindow::maskTolerance1(QPoint point)
{
    int x = point.x();
    int y = point.y();
    L_INFO_P("Starting the fill");

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    TolerancePredicate predicate(mMask, currentColor, mUi->toleranceSpinBox->value());
    AbstractPainter<RGB24Buffer> painter(mImage);
    painter.floodFill(x,y, predicate);
    L_INFO_P("Fill finished");
}

/* Final solutions */

class ToleranceFinalPredicate {
private:
    G8Buffer *mMask;
    vector<RGBColor>  mStartColor;

    int       mTolerance;
    QRect mLimit;

public:
    ToleranceFinalPredicate (
            G8Buffer *mask,
            RGBColor startColor,
            int tolerance,
            QRect limit) :
        mMask(mask),
        mTolerance(tolerance),
        mLimit(limit)
    {
        mStartColor.push_back(startColor);
    }

    ToleranceFinalPredicate (
            G8Buffer *mask,
            const vector<RGBColor> &startColor,
            int tolerance,
            QRect limit) :
        mMask(mask),
        mStartColor(startColor),
        mTolerance(tolerance),
        mLimit(limit)
    {}

    bool operator()(RGB24Buffer *buffer, int x, int y) {
        if (mMask->element(y,x) == 255)
            return false;

        if (!mLimit.contains(x,y))
            return false;

        RGBColor currentColor = buffer->element(y,x);
        for (unsigned i = 0; i < mStartColor.size(); i++)
        {
            RGBColor &color = mStartColor[i];
            int r = (int)currentColor.r() - (int)color.r();
            int g = (int)currentColor.g() - (int)color.g();
            int b = (int)currentColor.b() - (int)color.b();

            int sum = abs(r) + abs(g) + abs(b);
            if (sum < mTolerance) {
               return true;
            }
        }
        return false;
     }

     void mark(RGB24Buffer */*buffer*/, int x, int y) {
         mMask->element(y,x) = 255;
     }

};


void TestbedMainWindow::maskToleranceFinal(QPoint point)
{
    int x = point.x();
    int y = point.y();
    L_INFO_P("Starting the fill");

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);


    ToleranceFinalPredicate predicate(
            mMask,
            currentColor,
            mUi->toleranceSpinBox->value(),
            mImageWidget->getInputRect()
    );
    AbstractPainter<RGB24Buffer> painter(mImage);
    painter.floodFill(x,y, predicate);
    L_INFO_P("Fill finished");
}

void TestbedMainWindow::maskToleranceGraph(QPoint point)
{
    int x = point.x();
    int y = point.y();
    L_INFO_P("Starting the fill");

    //mGraphPlot->res
    if (!mMask->isValidCoord(y,x))
        return;

    vector<RGBColor> currentColor;
    switch (mUi->samplingComboBox->currentIndex())
    {
        case 0:
            currentColor.push_back(mImage->element(y,x));
            break;
        case 1:
            {
                int rad = mImageWidget->getImageRadius();
                qDebug() << "Radius is :" << rad;
                Vector3dd mean(0);
                int num = 0;

                for (int i = y - rad; i <= y + rad; i++ )
                {
                    double l = sqrt((rad * rad) - (i-y) * (i-y ));
                    for (int j = x - l; j <= x + l; j++)
                    {
                        if (!mImage->isValidCoord(i,j))
                        {
                            continue;
                        }

                        mean += mImage->element(i,j).toDouble();
                        num++;
                       // mImage->element(i,j) = RGBColor::Yellow();
                    }
                }

                mean /= num;
                currentColor.push_back(RGBColor(mean.x(), mean.y(), mean.z()));
                Vector2d<int> point = Vector2d<int>(x,y);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
            }
            break;
        case 2:
        default:
            {
                int rad = mImageWidget->getImageRadius();

                Vector2d<int> point;

                point = Vector2d<int>(x,y);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x + rad ,y);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x - rad ,y);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x,y + rad);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x,y + rad);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));

                point = Vector2d<int>(x + rad * 0.7 ,y + rad * 0.7);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x + rad * 0.7 ,y - rad * 0.7);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x - rad * 0.7 ,y - rad * 0.7);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));
                point = Vector2d<int>(x - rad * 0.7 ,y + rad * 0.7);
                if (mImage->isValidCoord(point)) currentColor.push_back(mImage->element(point));

                qDebug() << "Adding " << currentColor.size() << " points";
            }
            break;
    }


    QRect inputRect = mImageWidget->getInputRect();
    QRect maskRect = QRect(0,0,mMask->w - 1, mMask->h - 1);
    QRect workingRect = inputRect.intersected(maskRect);
    int totalArea = workingRect.height() * workingRect.width();

    vector<int> areas;
    G8Buffer *mMaskTmp = NULL;

    delete_safe(mMaskStore);
    mMaskStore = new G12Buffer(mMask->getSize());

    for (int i = 0; i < mUi->toleranceSpinBox->value(); i++)
    {
        delete_safe(mMaskTmp);
        mMaskTmp = new G8Buffer(mMask);
        ToleranceFinalPredicate predicate(
                mMaskTmp,
                currentColor,
                i,
                mImageWidget->getInputRect()
        );
        AbstractPainter<RGB24Buffer> painter(mImage);
        painter.floodFill(x,y, predicate);
        int area = mMaskTmp->countValues(255,
                workingRect.topLeft().x(),
                workingRect.topLeft().y(),
                workingRect.bottomRight().x(),
                workingRect.bottomRight().y()
        );
        qDebug() << "Processing tolerance:" << i << " area :" << area << " (" << ((double)area / totalArea) * 100 << "%)";
        areas.push_back(area);

        for (int y = 0; y < mMaskTmp->h; y++)
        {
            for (int x = 0; x < mMaskTmp->w; x++)
            {
                if (mMaskTmp->element(y,x) && mMaskStore->element(y,x) == 0) {
                    mMaskStore->element(y,x) = i;
                }
            }
        }

    }

    delete_safe(mMaskTmp);

    int toleranceMax = mUi->toleranceSpinBox->value();
    int toleranceMin = 0;
    for (int i = 0; i < areas.size() - 1 ; i++)
    {
        double part = ((double)areas[i + 1] / totalArea) * 100 ;
        double dpart = (((double)areas[i + 1] - areas[i] ) / totalArea) * 100;

        if (part > mUi->maximumSelectioSpinBox->value())
        {
            toleranceMax = i;
            break;
        }

        if (part > mUi->minimumSelectionSpinBox->value() && toleranceMin == 0)
        {
            toleranceMin = i;
        }

        if (part > mUi->minimumSelectionSpinBox->value() && dpart > mUi->spikeSpinBox->value())
        {
            toleranceMax = i - mUi->stepBackSpinBox->value();
            if (toleranceMax < toleranceMin) toleranceMax = toleranceMin;
            break;
        }
   }

    /* Draw result */
    mGraphPlot->addGraphPoint(0, 0, true);
    for (int i = areas.size() - 1; i >= 0; i--)
    {
        mGraphPlot->addGraphPoint(0, ((double)areas[i] / totalArea) * 100, true);

        if (i > 0)
        mGraphPlot->addGraphPoint(1, (((double)areas[i] - areas[i - 1]) / totalArea) * 100, true);

        if (i == toleranceMax) {
            mGraphPlot->addGraphPoint(2, 1, true);
        } else {
            mGraphPlot->addGraphPoint(2, 0, true);
        }

        mGraphPlot->addGraphPoint(3, mUi->spikeSpinBox->value(),true);

    }
    mGraphPlot->update();

    /*clean path*/
    mMaskTmp = new G8Buffer(mMask);
       ToleranceFinalPredicate predicate(
               mMaskTmp,
               currentColor,
               toleranceMax,
               mImageWidget->getInputRect()
       );
       AbstractPainter<RGB24Buffer> painter(mImage);
       painter.floodFill(x,y, predicate);

    mMask->fillWith(mMaskTmp);

    L_INFO_P("Fill finished");
}


/* Histogram */

class HistogramPredicate {
private:
    HistogramBuffer *mLocalHist;
    G8Buffer *mMask;
    LocalHistogram mBase;
    int       mTolerance;
public:
    HistogramPredicate (
            HistogramBuffer *localHist,
            G8Buffer *mask,
            LocalHistogram base,
            int tolerance) :
        mLocalHist(localHist), mMask(mask), mBase(base), mTolerance(tolerance) {};

    bool operator()(RGB24Buffer */*buffer*/, int x, int y) {
        if (mMask->element(y,x) == 255)
            return false;

        int sum = 0;
        for (int i = 0; i < LocalHistogram::SIZE; i++)
        {
            sum += abs(mBase.data[i] - mLocalHist->element(y,x).data[i]);
        }

        if (sum > mTolerance) {
           return false;
        }
        return true;
     }

     void mark(RGB24Buffer *buffer, int x, int y) {
         mMask->element(y,x) = 255;
     }

};


void TestbedMainWindow::maskLocalHist(QPoint point)
{
    int x = point.x();
    int y = point.y();
    L_INFO_P("Starting the fill");

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    HistogramPredicate predicate(mHistBuffer, mMask, mHistBuffer->element(y,x), mUi->histToleranceSpinBox->value());
    AbstractPainter<RGB24Buffer> painter(mImage);
    painter.floodFill(x,y, predicate);
    L_INFO_P("Fill finished");
}


class Histogram2DPredicate {
private:
    Histogram2DBuffer *mLocalHist;
    G8Buffer *mMask;
    LocalHistogram2D mBase;
    int       mTolerance;
public:
    Histogram2DPredicate (
            Histogram2DBuffer *localHist,
            G8Buffer *mask,
            LocalHistogram2D base,
            int tolerance) :
        mLocalHist(localHist), mMask(mask), mBase(base), mTolerance(tolerance) {};

    bool operator()(RGB24Buffer */*buffer*/, int x, int y) {
        if (mMask->element(y,x) == 255)
            return false;
        if (!mLocalHist->element(y,x).isSet)
            return false;

        int sum = 0;
        for (int i = 0; i < LocalHistogram2D::SIZE1 * LocalHistogram2D::SIZE2; i++)
        {
            sum += abs(mBase.data[i] - mLocalHist->element(y,x).data[i]);
        }

        if (sum > mTolerance) {
           return false;
        }
        return true;
     }

     void mark(RGB24Buffer */*buffer*/, int x, int y) {
         mMask->element(y,x) = 255;
     }

};

void TestbedMainWindow::maskLocalHist2D(QPoint point)
{
    int x = point.x();
    int y = point.y();
    L_INFO_P("Starting the fill");

    if (!mMask->isValidCoord(y,x))
        return;
    RGBColor currentColor = mImage->element(y,x);

    Histogram2DPredicate predicate(mHist2DBuffer, mMask, mHist2DBuffer->element(y,x), mUi->histToleranceSpinBox->value());
    AbstractPainter<RGB24Buffer> painter(mImage);
    painter.floodFill(x,y, predicate);
    L_INFO_P("Fill finished");
}


enum AlgorithmId { MASK_HUE, MASK_COLOR, MASK_COLOR_FAST, MASK_HISTOGRAM, MASK_HISTOGRAM_2D};

void TestbedMainWindow::doMask(QPoint point, int id)
{
    qDebug() << "TestbedMainWindow::doMask(): Point Selected(" << point << "," << id << ")";
    mUndoList.push_back(new G8Buffer(mMask));
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    if (id == 1)
    {
        maskToleranceGraph(point);
    } else {
        switch (mUi->maskComboBox->currentIndex())
        {
            case MASK_HUE:
                maskHue(mUi->hue1SpinBox->value(), mUi->hue2SpinBox->value());
                break;
            case MASK_COLOR:
                maskTolerance(point);
                break;
            case MASK_COLOR_FAST:
                maskTolerance1(point);
                break;
            case MASK_HISTOGRAM:
                maskLocalHist(point);
                break;
            case MASK_HISTOGRAM_2D:
                maskLocalHist2D(point);
                break;
        }
    }

    doFill();

    updateViewImage();
}

enum FillAlgorithmId { NO_FILL, FILL_MORHO, FILL_SIZE, FILL_MORHO_FAST};


void TestbedMainWindow::doFill()
{
    switch (mUi->postFillComboBox->currentIndex()) {
        case NO_FILL:
            break;
        case FILL_MORHO:
            doFillGapsMorpho();
            break;
        case FILL_SIZE:
            doFillGapsSegment();
            break;
        case FILL_MORHO_FAST:
            doFillGapsMorphoFast();
            break;
    }
}

void TestbedMainWindow::paramChanged()
{
    qDebug("TestbedMainWindow::paramChanged(): called\n");
}

void TestbedMainWindow::doErease(QPoint point)
{
    mUndoList.push_back(new G8Buffer(mMask));
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    AbstractPainter<G8Buffer> painter(mMask);
    painter.drawCircle(point.x(), point.y(), mUi->eraserSizeSpinBox->value(), 0);
    updateViewImage();
}


void TestbedMainWindow::doFillGapsMorpho()
{
    int size = mUi->fillCloseSizeSpinBox->value();
    L_INFO_P("Size %d", size);

    G12Buffer *core = new G12Buffer(2*size + 1, 2*size + 1);
    AbstractPainter<G12Buffer>(core).drawCircle(size, size, size, 0xFFF);
    BMPLoader().save("eandd_kernel.bmp", core);

    G8Buffer *dilated = Morphological::dilate (mMask, core, size, size);
    G8Buffer *closed = Morphological::erode  (dilated, core, size, size);
    delete_safe(mMask);
    mMask = closed;
    closed = NULL;
    delete_safe(dilated);
    delete_safe(closed);
    delete_safe(core);
}

void TestbedMainWindow::doFillGapsMorphoFast()
{
    int size = mUi->fillCloseSizeSpinBox->value();
    L_INFO_P("Size %d", size);

    qDebug("Called doFillGapsMorphoFast(): size is %d", size);

    G12Buffer *corev = new G12Buffer(2*size + 1, 1, false);
    G12Buffer *coreh = new G12Buffer(1, 2*size + 1, false);
    corev->fillWith(0xFF);
    coreh->fillWith(0xFF);

    BMPLoader().save("corev.bmp", corev);
    BMPLoader().save("coreh.bmp", coreh);

    G8Buffer *dilatedv = Morphological::dilate (mMask   , corev, 0, size);
    G8Buffer *dilated  = Morphological::dilate (dilatedv, coreh, size, 0);

    G8Buffer *closedv = Morphological::erode  (dilated, corev, 0, size);
    G8Buffer *closed  = Morphological::erode  (closedv, coreh, size, 0);

    delete_safe(mMask);
    mMask = closed;
    closed = NULL;
    delete_safe(closedv);
    delete_safe(dilated);
    delete_safe(dilatedv);

    delete_safe(coreh);
    delete_safe(corev);
}


void TestbedMainWindow::doFillGapsSegment()
{
    int size = mUi->fillGapSizeSpinBox->value();
    L_INFO_P("Size %d", size);

    GapsSegmentator segmentator;
    GapsSegmentator::SegmentationResult *result = segmentator.segment(mMask);

    /*sort(result->segments->begin(), result->segments->end(), GapSegment::sortPredicate);
    for (int i = 0; i < result->segments->size(); i++)
    {
        if (result->segments[i] > size)
            break;
    }*/
    /* Second path filling the gaps*/
    for (int i = 0; i < result->markup->h; i++)
    {
        for (int j = 0; j < result->markup->w; j++)
        {
            if (result->markup->element(i,j) == NULL)
                continue;
            if (result->markup->element(i,j)->size > size)
                continue;
            mMask->element(i,j) = true;
        }
    }
    delete_safe(result);
}


void TestbedMainWindow::doUpdateLocalHist(QPoint point)
{
    int x = point.x();
    int y = point.y();

    if (!mHistBuffer->isValidCoord(y,x)) {
        return;
    }

    LocalHistogram *hist = &mHistBuffer->element(y,x);
    const int columnWidth = 32;
    const int height = 255;
    int max = 1;
    for (int i = 0; i < 8; i++)
    {
        if (hist->data[i] > max)
            max = hist->data[i];
    }

    QImage *image = new QImage(hist->SIZE * columnWidth, height, QImage::Format_RGB32);
    image->fill(Qt::black);
    QPainter paint(image);
    paint.setPen(Qt::red);
    for (int i = 0; i < 8; i++)
    {
        int colHeight = (double)hist->data[i] / max * height;
        paint.drawRect(i * columnWidth, height - colHeight, columnWidth - 1, colHeight);
        printf("%d ", hist->data[i]);
    }
    paint.end();
    printf("\n");
    mLocalHistWidget->setImage(QSharedPointer<QImage>(image));
}

void TestbedMainWindow::doUpdateLocalHist2D(QPoint point)
{
    int x = point.x();
    int y = point.y();

    if (!mHist2DBuffer->isValidCoord(y,x)) {
        return;
    }

    LocalHistogram2D *hist = &mHist2DBuffer->element(y,x);
    const int squareSize = 32;

    int max = 1;
    for (int i = 0; i < hist->SIZE1 * hist->SIZE2 ; i++)
    {
        if (hist->data[i] > max)
            max = hist->data[i];
    }

    QImage *image = new QImage(hist->SIZE1 * squareSize, hist->SIZE2 * squareSize, QImage::Format_RGB32);
    image->fill(Qt::black);
    QPainter paint(image);
    printf("==\n");

    for (int i = 0; i < hist->SIZE1; i++) {
        for (int j = 0; j < hist->SIZE2; j++)
        {
            int offset = i * hist->SIZE2 + j;
            int value = (double)hist->data[offset] / max * 255;
            paint.setPen(Qt::blue);
            paint.setBrush(QColor(value, value, value));
            QRect rect(i * squareSize, j * squareSize, squareSize - 1,  squareSize - 1);
            paint.drawRect(rect);
            paint.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(hist->data[offset]));

            printf("%d ", hist->data[offset]);
        }
        printf("\n");
    }
    paint.end();
    printf("\n");
    mLocalHist2DWidget->setImage(QSharedPointer<QImage>(image));
}


void TestbedMainWindow::pointSelected(int toolID, QPoint point)
{
    switch (toolID)
    {
        case MASK_TOOL:
            doMask(point,0);
            break;
        case EREASE_TOOL:
            doErease(point);
            break;
        case HIST_INFO1:
            doUpdateLocalHist(point);
            doUpdateLocalHist2D(point);
            break;
        case HIST_INFO2:
            doUpdateLocalHist2D(point);
            break;
        case UNICOLOR_SELECTOR:
            qDebug() << "Final unicolor selector";
            doMask(point, 1);
            break;
        case TEXTURE_SELECTOR:
            doMask(point, 2);
            break;
    }
}

void TestbedMainWindow::pointSelectedMoved(int toolID, QPoint point)
{
    switch (toolID)
    {
        case EREASE_TOOL:
            doErease(point);
            break;
        case HIST_INFO1:
            doUpdateLocalHist(point);
            doUpdateLocalHist2D(point);
            break;
        case HIST_INFO2:
            doUpdateLocalHist2D(point);
            break;
    }
}



void TestbedMainWindow::rectSelected(int /*toolID*/, QRect rect)
{
    qDebug() << "Rect Selected: " << rect;
    mUndoList.push_back(new G8Buffer(mMask));
    if (mUndoList.size() > 20) {
        delete_safe(mUndoList.front());
        mUndoList.pop_front();
    }

    rect = rect.normalized();

    switch (mUi->maskComboBox->currentIndex())
    {
        case MASK_COLOR_FAST:
            for (int i = rect.top(); i < rect.bottom(); i++)
            {
                for (int j = rect.left(); j < rect.right(); j++)
                {
                    maskTolerance1(QPoint(j,i));
                }
            }
            break;
        case MASK_HISTOGRAM:
            for (int i = rect.top(); i < rect.bottom(); i++)
            {
                for (int j = rect.left(); j < rect.right(); j++)
                {
                    maskLocalHist(QPoint(j,i));
                }
            }
            break;
        case MASK_HISTOGRAM_2D:
            for (int i = rect.top(); i < rect.bottom(); i++)
            {
                for (int j = rect.left(); j < rect.right(); j++)
                {
                    maskLocalHist2D(QPoint(j,i));
                }
            }
            break;

    }

    doFill();

    updateViewImage();
}

TestbedMainWindow::~TestbedMainWindow()
{
    delete_safe(mImageWidget);
    delete_safe(mUi);
}
