#include <QTableWidgetItem>

#include "log.h"
#include "rgb24Buffer.h"
#include "curvatureFunc.h"
#include "radialFunc.h"
#include "qtHelper.h"

#include "camerasCalibration/camerasCalibrationFunc.h"
#include "lmDistortionSolver.h"

#include "g12Image.h"
#include "displacementBuffer.h"
#include "angleFunction.h"
#include "anglePointsFunction.h"
#include "distortionWidget.h"
#include "ui_distortionWidget.h"
#include "distPointsFunction.h"

#if NONFREE
#include "chessBoardDetector.h"
#endif

#ifdef WITH_OPENCV
# include "opencv2/imgproc/imgproc.hpp"
# include "opencv2/highgui/highgui.hpp"
# include <opencv2/calib3d/calib3d.hpp>
# include "opencv2/core/core_c.h"
# include "OpenCVTools.h"

# include "openCvCheckerboardDetector.h"


using namespace cv;

#endif

using corecvs::DistPointsFunction;


const double EPSILON = 0.0005;

DistortionWidget::DistortionWidget(QWidget *parent) :
      QWidget(parent)
    , mBufferInput(NULL)
    , mBufferWithCorners(NULL)
    , mDistortionParameters(NULL)
    , mDistortionCorrectTransform(NULL)
    , mResult(NULL)
    , mUi(new Ui::DistortionWidget)
{
    mUi->setupUi(this);
    this->setWindowTitle("Distortion correction");

    connect(mUi->widget, SIGNAL(newPolygonPointSelected(int,QPointF)), this, SLOT(tryAddPointToPolygon(int,QPointF)));
    connect(mUi->widget, SIGNAL(newPointFSelected(int, QPointF)), this, SLOT(tryAddPoint(int, QPointF)));
    connect(mUi->widget, SIGNAL(existedPointSelected(int, QPointF)), this, SLOT(initExistingPoint(int,QPointF)));
    connect(mUi->widget, SIGNAL(editPoint(QPointF, QPointF)), this, SLOT(editPoint(QPointF,QPointF)));

  //  connect(mUi->pointsTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(choosePoint(int,int)));
  //  connect(mUi->saveButton,             SIGNAL(released()), this, SLOT(addVector()));
  //  connect(mUi->deleteButton,           SIGNAL(released()), this, SLOT(deletePointPair()));

    connect(mUi->recalculateButton,      SIGNAL(released()), this, SLOT(initTransform()));
    connect(mUi->resetButton,            SIGNAL(released()), this, SLOT(resetParameters()));
    connect(mUi->setParamsButton,        SIGNAL(released()), this, SLOT(setParams()));
    connect(mUi->cornerDetectorButton,   SIGNAL(released()), this, SLOT(detectCorners()));
    connect(mUi->crossDetectorButton,    SIGNAL(released()), this, SLOT(detectCheckerboard()));
    connect(mUi->updateScoreButton,      SIGNAL(released()), this, SLOT(updateScore()));
    mDistortionParameters = new DistortionParameters();

    CheckerboardDetectionParameters params;
    mUi->checkerboardParametersWidget->setParameters(params);
    //mUi->calibrationFeatures->loadPoints();

    mObservationListModel.mObservationList = &mObservationList;
    mUi->calibrationFeatures->setObservationModel(&mObservationListModel);
}

void DistortionWidget::resetParameters()
{
    qDebug() << "DistortionWidget::resetParameters(): resetting parameters";
    mUi->bufferSelectorBox->setCurrentIndex(0);
    clearParameters();
    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(mBufferInput->h, mBufferInput->w)
    );
    emit recalculationFinished(mDistortionCorrectTransform);
}

DistortionWidget::~DistortionWidget()
{
    mUi->calibrationFeatures->savePoints();

    mDistortionCorrectTransform.clear();
    delete_safe(mResult);
    delete_safe(mDistortionParameters);
    delete_safe(mBufferWithCorners);
    delete_safe(mBufferInput);
    delete_safe(mUi);
}

void DistortionWidget::setParams()
{
    delete_safe(mDistortionParameters);
    mDistortionParameters = new DistortionParameters(
            mUi->radiusSpinBox->value(),
            mUi->dsbScale->value());
}

void DistortionWidget::initTransform()
{
    setCursor(Qt::BusyCursor);
    switch (mUi->methodSelectorBox->currentIndex())
    {
    case 0:
        doTransformLM();
        break;
    case 1:
        doInversionTransform();
        break;
    case 2:
        doLinesTransform();
        break;
    case 3:
        doDefaultTransform();
        break;
    case 4:
        doManualTransform();
        break;
    }
    unsetCursor();
    mUi->bufferSelectorBox->setCurrentIndex(2);
    showBufferChanged();

    emit recalculationFinished(mDistortionCorrectTransform);
}

void DistortionWidget::clearParameters()
{
   /* mUi->widget->clear();
    mUi->calibrationFeatures->clearObservationPoints();*/

    delete mDistortionParameters;
    mDistortionParameters = new DistortionParameters();
}


/**
 *  Distortion reconstruction with 3D template
 **/

/**
 *  Using corner detector as a helper
 **/

void DistortionWidget::detectCorners()
{
    if(mBufferInput == NULL) {
        return;
    }

    G12Buffer *grayChannel = mBufferInput->toG12Buffer();
    SpatialGradient *grad = new SpatialGradient(grayChannel);

    delete_safe(mBufferWithCorners);
    G12Buffer *corners = grad->findCornerPoints(mUi->lambdaDivisorBox->value());
    mUi->bufferSelectorBox->setCurrentIndex(1);

    CornerSegmentator segmentator(mUi->cornerThresholdBox->value());
    mResult = segmentator.segment<G12Buffer>(corners);
    foreach (CornerSegment *cornerSegment, *mResult->segments)
    {
        addPointPair(Vector3dd(), cornerSegment->getMean());
    }

    mBufferWithCorners = new RGB24Buffer(corners);

    delete_safe(corners);
    delete_safe(grad);
    delete_safe(grayChannel);
}

void DistortionWidget::detectCheckerboard()
{
#ifdef WITH_OPENCV
    CheckerboardDetectionParameters params;
    mUi->checkerboardParametersWidget->getParameters(params);

    if(mBufferInput == NULL) {
        return;
    }

    G8Buffer *workChannel = mBufferInput->getChannel(params.channel());

    PaintImageWidget *canvas = mUi->widget;

    if(params.cleanExisting())
    {
       // mUi->calibrationFeatures->clearObservationPoints();
        canvas->mFeatures.mPaths.clear();
    }

#define  OPENCV_DETECTOR
#ifdef OPENCV_DETECTOR
    OpenCvCheckerboardDetector detector(params);
#else
    ChessBoardDetectorParams cdp;
    cdp.w = params.mHorCrossesCount;
    cdp.h = params.mVertCrossesCount;
    ChessboardDetector detector(cdp);
#endif
    PatternDetector& patternDetector = detector;
    // FIXME: Not sure if we should ever allow user to tune what channel to use, let us just pass full buffer
    // TODO:  Check if drawing points over buffer is detector's part of work
    bool found = patternDetector.detectPattern(*mBufferInput);
    if (found)
    {
        mObservationListModel.clearObservationPoints();
        patternDetector.getPointData(mObservationList);
        mObservationListModel.setObservationList(&mObservationList);

        patternDetector.getPointData(canvas->mFeatures);
    }

    delete_safe(workChannel);
    return;
#else
    return;
#endif
}

void DistortionWidget::tryAddPoint(int toolID, const QPointF &point)
{
    if (toolID != 0)
        return;
    mUi->calibrationFeatures->manualAddPoint(Qt2Core::Vector2ddFromQPointF(point));
}

void DistortionWidget::initExistingPoint(int toolID, const QPointF & /*point*/)
{
    if (toolID != 0) {
        return;
    }

    /*!!!!!!!*/
    /*
    for (int i = 0; i < mUi->pointsTableWidget->rowCount(); i ++)
    {
        QPointF tablePoint(mUi->pointsTableWidget->item(i, 3)->text().toDouble(), mUi->pointsTableWidget->item(i, 4)->text().toDouble());
        if ((point - tablePoint).manhattanLength() < EPSILON)
        {
            choosePoint(i, 0);
            return;
        }
    }
    */
}

void DistortionWidget::setBuffer(G12Buffer *buffer)
{
    if (buffer == NULL) {
        return;
    }
    delete_safe(mBufferInput);
    mBufferInput = new RGB24Buffer(buffer);
    showBufferChanged();
}

void DistortionWidget::setBuffer(RGB24Buffer *buffer)
{
    if (buffer == NULL) {
        return;
    }
    delete_safe(mBufferInput);
    mBufferInput = new RGB24Buffer(buffer);
    showBufferChanged();
}




void DistortionWidget::doTransformLM()
{
    qDebug() << "DistortionWidget::doTransformLM(): Starting LM transform" << endl;
    LMDistortionSolver solver;
    solver.initialCenter = Vector2dd(mUi->widget->mImage.data()->width() * 0.5, mUi->widget->mImage.data()->height() * 0.5);;
    solver.list = &mObservationList;

    RadialCorrection correction = solver.solve();


    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(&correction, mBufferInput->h, mBufferInput->w, false)
    );
}

/**
 *   Correction by Maria Osechkina's algorithm
 **/

void DistortionWidget::doInversionTransform()
{
    qDebug() << "DistortionWidget::doInversionTransform(): Starting Inverstion transform" << endl;

    Vector2dd centre(mUi->widget->mImage.data()->width() * 0.5, mUi->widget->mImage.data()->height() * 0.5);
    vector<Vector2dd> points = mDistortionParameters->getPoints();

    DistortionCorrectTransform *transform = NULL;

    if (points.size() < 3 || !mDistortionParameters->needCalculateParams())
    {
        transform = new DistortionCorrectTransform(
            centre,
            mDistortionParameters->getRadius(),
            mDistortionParameters->getScale());
    }
    else
    {
        transform = new DistortionCorrectTransform(centre);
        transform->setRadius(points.at(0), points.at(1), points.at(2));
    }

    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(transform, mBufferInput->h, mBufferInput->w)
    );
    delete_safe(transform);
}

/**
 *   Correction by algorithm of straight
 **/
void DistortionWidget::doLinesTransform()
{
    LineDistortionEstimatorParameters params;
    mUi->lineDistortionWidget->getParameters(params);

    LMLinesDistortionSolver solver;
    solver.initialCenter = Vector2dd(mBufferInput->w / 2.0, mBufferInput->h /2.0);
    solver.lineList = &mUi->widget->mFeatures;
    solver.parameters = params;

    RadialCorrection correction = solver.solve();

    L_INFO_P("Inverting the distortion buffer");

    cout << "DistortionWidget::doLinesTransform():" << correction.mParams << std::endl;
    DisplacementBuffer *transform =
            DisplacementBuffer::CacheInverse(
            &correction,
            mBufferInput->h, mBufferInput->w,
            0.0, 0.0,
            (double)mBufferInput->w, (double)mBufferInput->h,
            0.5, 0);

    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(transform);
    mUi->isInverseCheckBox->setChecked(true);

    mUi->lensCorrectionWidget->setParameters(correction.mParams);
    L_INFO_P("Ending distortion calibration");
    updateScore();
}

void DistortionWidget::updateScore()
{
    LineDistortionEstimatorParameters params;
    mUi->lineDistortionWidget->getParameters(params);


    RadialCorrection corrector(mUi->lensCorrectionWidget->getParameters());

    LMLinesDistortionSolver solver;
    solver.initialCenter =  Vector2dd(mBufferInput->w / 2.0, mBufferInput->h /2.0);
    solver.lineList = &mUi->widget->mFeatures;
    solver.parameters = params;
    solver.computeCosts(corrector, true);

    mUi->widget->update();
  /*  mUi->scoreLabel->setText( QString("Line:%1px  Joint:%2")
            .arg(solver.costs[LineDistortionEstimatorCost::LINE_DEVIATION_COST])
            .arg(solver.costs[LineDistortionEstimatorCost::JOINT_ANGLE_COST])
        );*/

    EllipticalApproximation1d &cost = solver.costs[LineDistortionEstimatorCost::LINE_DEVIATION_COST];
    mUi->scoreLabel->setText( QString("Avg sq Err:%1px  MaxErr:%2px")
               .arg(cost.getRadiusAround0())
               .arg(cost.getMax()));

}

QSharedPointer<DisplacementBuffer> DistortionWidget::distortionCorrectionTransform()
{
    return mDistortionCorrectTransform;
}

void DistortionWidget::showBufferChanged()
{
    int id = mUi->bufferSelectorBox->currentIndex();
    RGB24Image *image = NULL;
    switch (id)
    {
    case 0:
        if (mBufferInput != NULL)
        {
            image = new RGB24Image(mBufferInput);
            mUi->widget->setImage(QSharedPointer<QImage>(image));
        }
        break;
    case 1:
        if (mBufferWithCorners != NULL)
        {
            image = new RGB24Image(mBufferWithCorners);
            mUi->widget->setImage(QSharedPointer<QImage>(image));
        }
        break;
    case 2:
        {
            if (mDistortionCorrectTransform == NULL) {
                break;
            }

            FixedPointDisplace displace(*mDistortionCorrectTransform, mDistortionCorrectTransform->h, mDistortionCorrectTransform->w);
            RGB24Buffer *buffer = mBufferInput->doReverseDeformationBlPrecomp(&displace);

            image = new RGB24Image(buffer);
            QPainter painter(image);
            vector<vector<Vector2dd> > straights = mUi->widget->mFeatures.getLines();

            RadialCorrection linesRadialCoorection;
            mUi->lensCorrectionWidget->getParameters(linesRadialCoorection.mParams);

            for (unsigned i = 0; i < straights.size(); i++)
            {
                for (unsigned j = 0; j < straights[i].size(); j++)
                {
                    Vector2dd point = straights[i][j];
                    Vector2dd moved = linesRadialCoorection.map(point.y(), point.x());
                    painter.setPen(Qt::yellow);
                    painter.drawLine(moved.x() - 1, moved.y() - 1, moved.x() + 1, moved.y() + 1);
                    painter.drawLine(moved.x() + 1, moved.y() - 1, moved.x() - 1, moved.y() + 1);
//                    cout << point << "=>" << moved << "\n";
                }
            }
            painter.end();

            mUi->widget->setImage(QSharedPointer<QImage>(image));
            delete_safe(buffer);
            break;
        }
    }
}


void DistortionWidget::addPointPair(const Vector3dd &key, const Vector2dd &value)
{
    PointObservation observation(key, value);
    mObservationListModel.setObservationList(NULL);
    mObservationList.push_back(observation);
    mObservationListModel.setObservationList(&mObservationList);
    mUi->widget->mFeatures.appendNewVertex(value);
}


void DistortionWidget::doDefaultTransform()
{
    const double scale = 1.05;
    const Vector2dd  center(mBufferInput->w * 0.5, mBufferInput->h * 0.5);
    const double k = 1024.0 / mBufferInput->h; //params for ueyecameras
    const double params[] = {
            0.000657414 * k,
           -6.97987e-05 * k * k,
             7.9655e-07 * pow(k, 3),
             -4.318e-09 * pow(k, 4),
            1.33988e-11 * pow(k, 5),
           -2.50562e-14 * pow(k, 6),
            2.79465e-17 * pow(k, 7),
           -1.71374e-20 * pow(k, 8),
             4.4494e-24 * pow(k, 9)};
    vector<double> straightParams(params, params + CORE_COUNT_OF(params));
    LensDistortionModelParameters lenCorrectionParams(
                center.x(),
                center.y(),
                0.0, 0.0,
                straightParams,
                1.0,
                scale,
                center.l2Metric()
             );

    RadialCorrection radCorrection(lenCorrectionParams);
    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(&radCorrection, mBufferInput->h, mBufferInput->w, true)
    );
}

/* Manual transform */

void DistortionWidget::doManualTransform()
{
    qDebug() << "DistortionWidget::doManualTransform(): Starting Manual transform" << endl;

    RadialCorrection linesRadialCoorection;
    mUi->lensCorrectionWidget->getParameters(linesRadialCoorection.mParams);

    if (!mUi->isInverseCheckBox->isChecked()) {
        mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(new DisplacementBuffer(&linesRadialCoorection, mBufferInput->h, mBufferInput->w, true));
    } else {
        mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(DisplacementBuffer::CacheInverse(&linesRadialCoorection,
            mBufferInput->h, mBufferInput->w,
            0.0,0.0,
            (double)mBufferInput->w, (double)mBufferInput->h, 0.5)
        );
    }
    updateScore();
}


