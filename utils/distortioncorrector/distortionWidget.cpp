#include <QtGui/QTableWidgetItem>

#include "log.h"
#include "rgb24Buffer.h"
#include "straightFunc.h"
#include "curvatureFunc.h"
#include "radialFunc.h"

#include "camerasCalibration/camerasCalibrationFunc.h"

#include "g12Image.h"
#include "displacementBuffer.h"
#include "angleFunction.h"
#include "anglePointsFunction.h"
#include "distortionWidget.h"
#include "ui_distortionWidget.h"
#include "distPointsFunction.h"

using corecvs::DistPointsFunction;


typedef QPair<Vector3dd, Vector2dd> Points;
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
    mUi->imageXSpinBox->setMaximum(2000);
    mUi->imageYSpinBox->setMaximum(2000);
    mUi->xCoordSpinBox->setMaximum(2000);
    mUi->yCoordSpinBox->setMaximum(2000);
    mUi->zCoordSpinBox->setMaximum(2000);

    connect(mUi->widget, SIGNAL(newPolygonPointSelected(int,QPointF)), this, SLOT(tryAddPointToPolygon(int,QPointF)));
    connect(mUi->widget, SIGNAL(newPointFSelected(int, QPointF)), this, SLOT(tryAddPoint(int, QPointF)));
    connect(mUi->widget, SIGNAL(existedPointSelected(int, QPointF)), this, SLOT(initExistingPoint(int,QPointF)));
    connect(mUi->widget, SIGNAL(editPoint(QPointF, QPointF)), this, SLOT(editPoint(QPointF,QPointF)));

    connect(mUi->pointsTableWidget, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(choosePoint(int,int)));
    connect(mUi->recalculateButton,      SIGNAL(released()), this, SLOT(initTransform()));
    connect(mUi->resetButton,            SIGNAL(released()), this, SLOT(resetParameters()));
    connect(mUi->setParamsButton,        SIGNAL(released()), this, SLOT(setParams()));
    connect(mUi->saveButton,             SIGNAL(released()), this, SLOT(addVector()));
    connect(mUi->cornerDetectorButton,   SIGNAL(released()), this, SLOT(detectCorners()));
    connect(mUi->updateScoreButton,      SIGNAL(released()), this, SLOT(updateScore()));
    connect(mUi->deleteButton,           SIGNAL(released()), this, SLOT(deletePointPair()));
    mDistortionParameters = new DistortionParameters();

    /* Additional subwidgets */
    connect(mUi->graphButton, SIGNAL(released()), &mGraphDialog, SLOT(show()));
    connect(mUi->levelGraphButton, SIGNAL(released()), &mLevelGraphDialog, SLOT(show()));

    connect(mUi->addPowerButton, SIGNAL(released()), this, SLOT(addPower()));
    connect(mUi->delPowerButton, SIGNAL(released()), this, SLOT(delPower()));

    loadPoints();
}

void DistortionWidget::resetParameters()
{
    mUi->bufferSelectorBox->setCurrentIndex(0);
    clearParameters();
    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(mBufferInput->h, mBufferInput->w)
    );
    emit recalculationFinished(mDistortionCorrectTransform);
}

DistortionWidget::~DistortionWidget()
{
    savePoints();

    mDistortionCorrectTransform.clear();
    delete_safe(mResult);
    delete_safe(mDistortionParameters);
    delete_safe(mBufferWithCorners);
    delete_safe(mBufferInput);
    delete_safe(mUi);
}

void DistortionWidget::setParams()
{
    delete mDistortionParameters;
    mDistortionParameters = new DistortionParameters(
            mUi->radiusSpinBox->value(),
            mUi->scaleSpinBox->value());
}

void DistortionWidget::initTransform()
{
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
    mUi->bufferSelectorBox->setCurrentIndex(2);
    showBufferChanged();
    emit recalculationFinished(mDistortionCorrectTransform);
}

void DistortionWidget::clearParameters()
{
    mUi->widget->clear();

//    mNewStraight.clear();
//    mStraights.clear();
    mCorrectionMap.clear();
    int row = mUi->pointsTableWidget->rowCount();
    for (int i = 0; i < row; i ++)
    {
        mUi->pointsTableWidget->removeRow(0);
    }

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
    SpatialGradient *grad = new SpatialGradient(mBufferInput);

    delete mBufferWithCorners;
    mBufferWithCorners = grad->findCornerPoints(mUi->lambdaDivisorBox->value());
    mUi->bufferSelectorBox->setCurrentIndex(1);

    CornerSegmentator segmentator(mUi->cornerThresholdBox->value());
    mResult = segmentator.segment<G12Buffer>(mBufferWithCorners);
    foreach (CornerSegment *cornerSegment, *mResult->segments)
    {
        addPointPair(Vector3dd(), cornerSegment->getMean());
    }
    delete_safe(grad);
}

void DistortionWidget::tryAddPoint(int toolID, const QPointF &point)
{
    if (toolID != 0)
        return;
    mUi->imageXSpinBox->setValue(point.x());
    mUi->imageYSpinBox->setValue(point.y());
}

void DistortionWidget::initExistingPoint(int toolID, const QPointF &point)
{
    if (toolID != 0) {
        return;
    }
    for (int i = 0; i < mUi->pointsTableWidget->rowCount(); i ++)
    {
        QPointF tablePoint(mUi->pointsTableWidget->item(i, 3)->text().toDouble(), mUi->pointsTableWidget->item(i, 4)->text().toDouble());
        if ((point - tablePoint).manhattanLength() < EPSILON)
        {
            choosePoint(i, 0);
            return;
        }
    }
}

void DistortionWidget::setBuffer(G12Buffer *buffer)
{
    if (buffer == NULL) {
        return;
    }
    delete mBufferInput;
    mBufferInput = new G12Buffer(buffer);
    showBufferChanged();
}


void DistortionWidget::addVector()
{
    Vector3dd key(mUi->xCoordSpinBox->value(), mUi->yCoordSpinBox->value(), mUi->zCoordSpinBox->value());
    Vector2dd value(mUi->imageXSpinBox->value(), mUi->imageYSpinBox->value());
    addPointPair(key, value);
}

void DistortionWidget::doTransformLM()
{
    Vector2d32 center(mUi->widget->mImage.data()->width() * 0.5, mUi->widget->mImage.data()->height() * 0.5);
    vector<double> values;
    vector<Vector3dd> arguments;
    foreach (Points points, mCorrectionMap)
    {
        values.push_back(points.second.x());
        values.push_back(points.second.y());
        arguments.push_back(points.first);
    }
    CamerasCalibrationFunc func(&arguments, center);
    vector<double> bestParams;

    double params[] = {0, 0, 0, 0, -5.09, 11.1, -30.4, 35, 0.12, 0.04, -4.81, 14.6, -11.2, 11.2};
    vector<double> parameters(params, params + CORE_COUNT_OF(params));
    LevenbergMarquardt LMTransform(10000, 4, 1.2);
    LMTransform.f = &func;
    func.setIsDistored(false);
    bestParams = LMTransform.fit(parameters,values);
    func.setIsDistored(true);
    bestParams = LMTransform.fit(bestParams, values);
    vector<double> polynomKoeff;
    polynomKoeff.push_back(bestParams.at(0));
    polynomKoeff.push_back(bestParams.at(1));
    LensCorrectionParametres lenCorrectParams(polynomKoeff, bestParams.at(2), bestParams.at(3), center);

    RadialCorrection correction(lenCorrectParams);
    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(&correction, mBufferInput->h, mBufferInput->w, false)
    );
}

/**
 *   Correction by Maria Osechkina's algorithm
 **/

void DistortionWidget::doInversionTransform()
{
    Vector2dd centre(mUi->widget->mImage.data()->width() * 0.5, mUi->widget->mImage.data()->height() * 0.5);
    vector<Vector2dd> points = mDistortionParameters->getPoints();

    DistortionCorrectTransform *transform = new DistortionCorrectTransform(centre);
    if (points.size() < 3 || !mDistortionParameters->needCalculateParams())
    {
        delete transform;
        transform = new DistortionCorrectTransform(
            centre,
            mDistortionParameters->getRadius(),
            mDistortionParameters->getScale());
    }
    else
    {
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
    double scale = mUi->lineScaleSpinBox->value();
    qDebug() << "doLinesTransform: aspectCorrection = "<< scale;

    Vector2dd center(mBufferInput->w / 2.0, mBufferInput->h /2.0);

    PaintImageWidget *editor = mUi->widget;
    vector<vector<Vector2dd> > straights = editor->getPaths();
    L_INFO_P("Starting distortion calibration on %d lines", straights.size());

    RadialCorrection correction(LensCorrectionParametres(
       vector<double>(mUi->degreeSpinBox->value()),
       0.0, 0.0,
       1.0,
       center.l2Metric(),
       center
    ));

    ModelToRadialCorrection modelFactory(
        correction,
        mUi->estimateCenterCheckBox->isChecked(),
        mUi->estimateTangentCheckBox->isChecked(),
        mUi->degreeSpinBox->value()
    );

    FunctionArgs *costFuntion = NULL;
    if (mUi->costFunctionComboBox->currentIndex() == 0) {
        costFuntion = new AnglePointsFunction (straights, modelFactory);
        //straightF.simpleJacobian = mUi->simpleJacobianCheckBox->isChecked();
    } else {
        costFuntion = new DistPointsFunction  (straights, modelFactory);
    }

    LevenbergMarquardt straightLevMarq(mUi->iterationsSpinBox->value(), 2, 1.5);
    straightLevMarq.f = costFuntion;

    /* First aproximation is zero vector */
    vector<double> first(costFuntion->inputs, 0);
    modelFactory.getModel(correction, &first[0]);

    vector<double> value(costFuntion->outputs, 0);
    vector<double> straightParams = straightLevMarq.fit(first, value);

    mLinesRadialCoorection = modelFactory.getRadial(&straightParams[0]);

    L_INFO_P("Inverting the distortion buffer");

   mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(DisplacementBuffer::CacheInverse(&mLinesRadialCoorection,
            mBufferInput->h, mBufferInput->w,
            0.0,0.0,
            (double)mBufferInput->w, (double)mBufferInput->h,
            0.5, mUi->preciseInvertionCheckBox->isChecked())
   );

    mUi->isInverseCheckBox->setChecked(true);
    updateAdditionalData();
    L_INFO_P("Ending distortion calibration");
    updateScore();

}

void DistortionWidget::updateScore()
{
    PaintImageWidget *editor = mUi->widget;
    vector<vector<Vector2dd> > straights = editor->getPaths();

    ModelToRadialCorrection modelFactory(
        mLinesRadialCoorection,
        mUi->estimateCenterCheckBox->isChecked(),
        mUi->estimateTangentCheckBox->isChecked(),
        mUi->degreeSpinBox->value()
    );

    FunctionArgs *costFuntion = NULL;
    if (mUi->costFunctionComboBox->currentIndex() == 0) {
        costFuntion = new AnglePointsFunction (straights, modelFactory);
           //straightF.simpleJacobian = mUi->simpleJacobianCheckBox->isChecked();
    } else {
        costFuntion = new DistPointsFunction  (straights, modelFactory);
    }

    vector<double> modelParameters(costFuntion->inputs, 0);
    modelFactory.getModel(mLinesRadialCoorection, &modelParameters[0]);

    vector<double> result(costFuntion->outputs);
    costFuntion->operator()(&modelParameters[0], &result[0]);

    double sqSum = 0;
    double maxValue = 0.0;
    for (unsigned i = 0; i < result.size(); i++) {
        if (fabs(result[i]) > maxValue) {
            maxValue = fabs(result[i]);
        }
        sqSum += result[i] * result[i];
    }

    int count = 0;
    PaintImageWidget *canvas = mUi->widget;
    for (unsigned i = 0; i < (unsigned)canvas->mPaths.size(); i++)
    {
        PaintImageWidget::VertexPath &path = canvas->mPaths[i];
        if (path.vertexes.size() < 3) {
            continue;
        }
        if (mUi->costFunctionComboBox->currentIndex() == 0)
        {
            for (int j = 1; j < path.vertexes.size() - 1; j++) {
                path.vertexes[j]->weight = fabs(result[count]) / maxValue;
                count++;
            }
        } else {
            for (int j = 0; j < path.vertexes.size(); j++) {
                path.vertexes[j]->weight = fabs(result[count]) / maxValue;
                count++;
            }
        }
    }
    canvas->update();

    sqSum /= result.size();

    mUi->scoreLabel->setText(QString::number(sqrt(sqSum)));
}

QSharedPointer<DisplacementBuffer> DistortionWidget::distortionCorrectionTransform()
{
    return mDistortionCorrectTransform;
}

void DistortionWidget::showBufferChanged()
{
    int id = mUi->bufferSelectorBox->currentIndex();
    G12Image *image = NULL;
    switch (id)
    {
    case 0:
        if (mBufferInput != NULL)
        {
            image = new G12Image(mBufferInput);
            mUi->widget->setImage(QSharedPointer<QImage>(image));
        }
        break;
    case 1:
        if (mBufferWithCorners != NULL)
        {
            image = new G12Image(mBufferWithCorners);
            mUi->widget->setImage(QSharedPointer<QImage>(image));
        }
        break;
    case 2:
        {
            if (mDistortionCorrectTransform == NULL) {
                break;
            }
            G12Buffer *buffer = mBufferInput->doReverseDeformationBl<G12Buffer, DisplacementBuffer>(
                        mDistortionCorrectTransform.data(), mBufferInput->h, mBufferInput->w);
            image = new G12Image(buffer);
            QPainter painter(image);
            vector<vector<Vector2dd> > straights = mUi->widget->getPaths();
            for (unsigned i = 0; i < straights.size(); i++) {
                for (unsigned j = 0; j < straights[i].size(); j++) {
                    Vector2dd point = straights[i][j];
                    Vector2dd moved = mLinesRadialCoorection.map(point.y(), point.x());
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

/*Seems like a trash*/
void DistortionWidget::printVectorPair(const Vector3dd &key, const Vector2dd &value)
{
    QTableWidget *table = mUi->pointsTableWidget;
    for (int i = 0; i < table->rowCount(); i ++)
    {
        if (table->item(i, 3)->text().toDouble() == value.x() &&
            table->item(i, 4)->text().toDouble() == value.y())
        {
            table->item(i, 0)->setText(QString::number(key.x()));
            table->item(i, 1)->setText(QString::number(key.y()));
            table->item(i, 2)->setText(QString::number(key.z()));
            return;
        }
    }
    for (int i = 0; i < table->rowCount(); i ++)
    {
        for (int j = 0; j < table->columnCount(); j ++)
            table->item(i, j)->setBackgroundColor(Qt::color0);
    }
    table->insertRow(mUi->pointsTableWidget->rowCount());
    int row = table->rowCount() - 1;
    table->setItem(row, 0, new QTableWidgetItem(QString::number(key.x())));
    table->setItem(row, 1, new QTableWidgetItem(QString::number(key.y())));
    table->setItem(row, 2, new QTableWidgetItem(QString::number(key.z())));
    table->setItem(row, 3, new QTableWidgetItem(QString::number(value.x())));
    table->setItem(row, 4, new QTableWidgetItem(QString::number(value.y())));

}


void DistortionWidget::addPointPair(const Vector3dd &key, const Vector2dd &value)
{
    unsigned i = 0;
    while (i < mCorrectionMap.size())
    {
        if ((mCorrectionMap.at(i).second - value).l2Metric() < EPSILON)
        {
            mCorrectionMap.erase(mCorrectionMap.begin() + i);
        } else {
            i ++;
        }
    }
    mCorrectionMap.push_back(QPair<Vector3dd,Vector2dd>(key, value));

    printVectorPair(key, value);

    mUi->widget->addVertex(value);

}

void DistortionWidget::choosePoint(int row, int /*column*/)
{
    QTableWidget *table = mUi->pointsTableWidget;

    for (int i = 0; i < table->rowCount(); i ++)
    {
        for (int j = 0; j < table->columnCount(); j ++)
        {
            table->item(i, j)->setBackgroundColor(Qt::color0);
        }
    }
    for (int i = 0; i < table->columnCount(); i ++)
    {
        table->item(row, i)->setBackgroundColor(Qt::yellow);
    }
    mUi->imageXSpinBox->setValue(table->item(row, 3)->text().toDouble());
    mUi->imageYSpinBox->setValue(table->item(row, 4)->text().toDouble());
    mUi->xCoordSpinBox->setValue(table->item(row, 0)->text().toDouble());
    mUi->yCoordSpinBox->setValue(table->item(row, 1)->text().toDouble());
    mUi->zCoordSpinBox->setValue(table->item(row, 2)->text().toDouble());
    //ui->widget->setCurrentPoint(QPointF(ui->dsbImageX->value(), ui->dsbImageY->value()));
}

void DistortionWidget::deletePointPair()
{
    QTableWidget *table = mUi->pointsTableWidget;
    Vector2dd value(mUi->imageXSpinBox->value(), mUi->imageYSpinBox->value());
    unsigned i = 0;
    while (i < mCorrectionMap.size())
    {
        if ((mCorrectionMap.at(i).second - value).l2Metric() < EPSILON)
        {
            mCorrectionMap.erase(mCorrectionMap.begin() + i);
        } else {
            i ++;
        }
    }

    for (int i = 0; i < table->rowCount(); i ++)
    {
        if (table->item(i, 3)->text().toDouble() == value.x() &&
            table->item(i, 4)->text().toDouble() == value.y())
        {
            table->removeRow(i);
            break;
        }
    }
    // TODO: PaintFeature
    // ui->widget->deletePoint(QPointF(value.x(), value.y()));
}

void DistortionWidget::editPoint(const QPointF &prevPoint, const QPointF &newPoint)
{
    QTableWidget *table = mUi->pointsTableWidget;
    for (int i = 0; i < table->rowCount(); i ++)
    {
        QPointF tablePoint(table->item(i, 3)->text().toDouble(), table->item(i, 4)->text().toDouble());
        if ((prevPoint - tablePoint).manhattanLength() < EPSILON)
        {
            table->item(i, 3)->setText(QString::number(newPoint.x()));
            table->item(i, 4)->setText(QString::number(newPoint.y()));
            choosePoint(i, 0);
        }
    }
    Vector2dd prevVector(prevPoint.x(), prevPoint.y());
    Vector3dd spacePos(-1, -1, -1);
    for (unsigned i = 0; i < mCorrectionMap.size(); i ++)
    {
        if ((mCorrectionMap.at(i).second - prevVector).l2Metric() < EPSILON)
        {
            spacePos = mCorrectionMap.at(i).first;
            mCorrectionMap.erase(mCorrectionMap.begin() + i);
            break;
        }
    }
    Vector2dd newVector(newPoint.x(), newPoint.y());
    mCorrectionMap.push_back(Points(spacePos, newVector));
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
    LensCorrectionParametres lenCorrectionParams(straightParams, 0, 0, /*p1, p2,*/ scale, 200.0,  center);
    RadialCorrection radCorrection(lenCorrectionParams);
    mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(
            new DisplacementBuffer(&radCorrection, mBufferInput->h, mBufferInput->w, true)
    );
}

/* Manual transform */

void DistortionWidget::doManualTransform()
{
    LensCorrectionParametres &lensParams = mLinesRadialCoorection.mParams;
    lensParams.center.x() = mUi->centerXSpinBox->value();
    lensParams.center.y() = mUi->centerYSpinBox->value();
    lensParams.p1 = mUi->tangential1SpinBox->value();
    lensParams.p2 = mUi->tangential2SpinBox->value();
    lensParams.aspect = mUi->scaleSpinBox->value();

    lensParams.koeff.clear();
    for (int i = 0; i < mUi->koefTableWidget->rowCount(); i ++) {
        QVariant value = mUi->koefTableWidget->item(i, 1)->data(Qt::DisplayRole);
        bool ok = true;
        double koef = value.toDouble(&ok);
        qDebug() << i << "(" << value << ") =" << koef << "[" << ok << "]";
        lensParams.koeff.push_back(koef);
    }

    mLinesRadialCoorection = RadialCorrection(lensParams);
    if (!mUi->isInverseCheckBox->isChecked()) {
        mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(new DisplacementBuffer(&mLinesRadialCoorection, mBufferInput->h, mBufferInput->w, true));
    } else {
        mDistortionCorrectTransform = QSharedPointer<DisplacementBuffer>(DisplacementBuffer::CacheInverse(&mLinesRadialCoorection,
            mBufferInput->h, mBufferInput->w,
            0.0,0.0,
            (double)mBufferInput->w, (double)mBufferInput->h, 0.5)
        );
    }
    updateScore();
    updateAdditionalData();
}


void DistortionWidget::updateAdditionalData()
{
    LensCorrectionParametres &lensParams = mLinesRadialCoorection.mParams;
    /* Updating graph */
    int diagonal = Vector2dd(mBufferInput->w, mBufferInput->h).l2Metric();
    for (int i = 0; i < diagonal; i++) {
        mGraphDialog.addGraphPoint(0, mLinesRadialCoorection.radialScale(((double)diagonal - i - 1) / diagonal), true);
    }
    mGraphDialog.update();

    /* Updating distortion*/
    Map2DFunction<DisplacementBuffer> mapFunc(mDistortionCorrectTransform.data());
    LengthFunction lengthFunc(&mapFunc);
    RGB24Buffer *output = new RGB24Buffer(mDistortionCorrectTransform->getSize());
    output->drawIsolines(0.0, 0.0,
            (double)mBufferInput->h,  (double)mBufferInput->w,
            0.1, lengthFunc);

    mLevelGraphDialog.setImage(QSharedPointer<QImage>(new RGB24Image(output)));
    delete_safe(output);

    /* Updating textual output */
    mUi->centerXSpinBox->setValue(lensParams.center.x());
    mUi->centerYSpinBox->setValue(lensParams.center.y());
    mUi->tangential1SpinBox->setValue(lensParams.p1);
    mUi->tangential2SpinBox->setValue(lensParams.p2);

    mUi->scaleSpinBox->setValue(lensParams.aspect);

    mUi->koefTableWidget->clear();
    mUi->koefTableWidget->setColumnCount(2);
    mUi->koefTableWidget->setRowCount(lensParams.koeff.size());
    for (unsigned i = 0; i < lensParams.koeff.size(); i ++) {
        mUi->koefTableWidget->setItem(i, 0, new QTableWidgetItem(QString("x^%1").arg(i + 1)));
        QVariant value(QString::number(lensParams.koeff[i], 'g', 15));
        QTableWidgetItem *data = new QTableWidgetItem();
        data->setData(Qt::DisplayRole, value);
        data->setFlags(data->flags() | Qt::ItemIsEditable);
        mUi->koefTableWidget->setItem(i, 1, data);
    }
}

void DistortionWidget::addPower()
{
    int newRow = mUi->koefTableWidget->rowCount() + 1;
    mUi->koefTableWidget->setRowCount(newRow);
    mUi->koefTableWidget->setColumnCount(2);
    mUi->koefTableWidget->setItem(newRow - 1, 0, new QTableWidgetItem(QString("x^%1").arg(newRow)));
}

void DistortionWidget::delPower()
{
    mUi->koefTableWidget->setRowCount(mUi->koefTableWidget->rowCount() - 1);
}

/**
 *   Loading and saving of current points
 **/

void DistortionWidget::savePoints()
{
    QSettings settings("distortionCorrection.conf", QSettings::IniFormat);
    settings.beginWriteArray("points");
    for (unsigned i = 0; i < mCorrectionMap.size(); i ++)
    {
        Points points = mCorrectionMap.at(i);
        settings.setArrayIndex(i);
        settings.setValue("spacePoint_X", points.first.x());
        settings.setValue("spacePoint_Y", points.first.y());
        settings.setValue("spacePoint_Z", points.first.z());
        settings.setValue("imagePoint_X", points.second.x());
        settings.setValue("imagePoint_Y", points.second.y());
    }
    settings.endArray();

    vector<vector<Vector2dd> > straights = mUi->widget->getPaths();
    settings.beginWriteArray("lines");
    for (unsigned i = 0; i < straights.size(); i++) {
        settings.setArrayIndex(i);
        settings.beginWriteArray("subpoints");
        for (unsigned j = 0; j < straights[i].size(); j++) {
            settings.setArrayIndex(j);

            settings.setValue("point.x", straights[i][j].x());
            settings.setValue("point.y", straights[i][j].y());
        }
        settings.endArray();
    }
    settings.endArray();
}

void DistortionWidget::loadPoints()
{
    QSettings settings("distortionCorrection.conf", QSettings::IniFormat);
    int size = settings.beginReadArray("points");
    for (int i = 0; i < size; i ++)
    {
        Points points;
        settings.setArrayIndex(i);
        points.first[0]  = settings.value("spacePoint_X").toDouble();
        points.first[1]  = settings.value("spacePoint_Y").toDouble();
        points.first[2]  = settings.value("spacePoint_Z").toDouble();
        points.second[0] = settings.value("imagePoint_X").toDouble();
        points.second[1] = settings.value("imagePoint_Y").toDouble();
        addPointPair(points.first, points.second);
    }
    settings.endArray();

    /* TODO: Make an interface for this */
    PaintImageWidget *canvas = mUi->widget;

    int lines = settings.beginReadArray("lines");
    for (int i = 0; i < lines; i++) {
        canvas->mPaths.append(PaintImageWidget::VertexPath());
        PaintImageWidget::VertexPath &path = canvas->mPaths.last();

        settings.setArrayIndex(i);
        int subpoints = settings.beginReadArray("subpoints");
        for (int j = 0; j < subpoints; j++) {
            settings.setArrayIndex(j);
            Vector2dd pos;
            pos.x() = settings.value("point.x").toDouble();
            pos.y() = settings.value("point.y").toDouble();
            canvas->addVertex(pos);
            canvas->addVertexToPath(&canvas->mPoints.last(), &path);
        }
        settings.endArray();
    }
    settings.endArray();
}
