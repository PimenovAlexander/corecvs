#pragma once

#include <QtGui/QWidget>

#include "advancedImageWidget.h"
#include "distortionParameters.h"
#include "vector3d.h"
#include "radialCorrection.h"
#include "distortionCorrectTransform.h"
#include "displacementBuffer.h"
#include "levenmarq.h"
#include "spatialGradient.h"
#include "segmentator.h"
#include "graphPlotDialog.h"

namespace Ui {
class DistortionWidget;
}

class DistortionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DistortionWidget(QWidget *parent = 0);
    ~DistortionWidget();
    //DistortionParameters *getDistortionParameters();
    QSharedPointer<DisplacementBuffer> distortionCorrectionTransform();
    void setBuffer(G12Buffer *buffer);

private:
    /* Images */
    G12Buffer *mBufferInput;
    G12Buffer *mBufferWithCorners;

    QList<QLine> mCorrectionVectors;
    vector<QPair<Vector3dd, Vector2dd> > mCorrectionMap;
    void setCameraParameters();

    /* Parameters */
    DistortionParameters *mDistortionParameters;
    void clearParameters();

    /* Methods to trigger recalculation */
    void doTransformLM();
    void doInversionTransform();
    void doLinesTransform();
    void doDefaultTransform();
    void doManualTransform();

    /* Current correction result in different forms */
    RadialCorrection mLinesRadialCoorection;
    QSharedPointer<DisplacementBuffer> mDistortionCorrectTransform;

    /* Some data for semiautomatic corner detection*/
    Segmentator<CornerSegmentator, CornerSegment>::SegmentationResult *mResult;

    void printVectorPair(const Vector3dd &spacePoint, const Vector2dd &imagePoint);
    void addPointPair(const Vector3dd &spacePoint, const Vector2dd &imagePoint);

    GraphPlotDialog     mGraphDialog;
    AdvancedImageWidget mLevelGraphDialog;

private slots:
//    void tryAddPointToPolygon(int toolID, QPointF const &point);
    void tryAddPoint(int toolID, QPointF const &point);
    void initExistingPoint(int toolID, QPointF const &point);
    void setParams();
    void addVector();
    void initTransform();
    void detectCorners();
    void resetParameters();
    void choosePoint(int row, int column);
    void showBufferChanged();
    void deletePointPair();
    void editPoint(QPointF const &prevPoint, QPointF const &newPoint);

    /*Manual input slots*/
    void addPower();
    void delPower();

    void updateScore();
    void updateAdditionalData();


    /**
     * Saving/loading state
     **/
    void savePoints();
    void loadPoints();

signals:
    void recalculationFinished(QSharedPointer<DisplacementBuffer> result);

private:
    Ui::DistortionWidget *mUi;
};
