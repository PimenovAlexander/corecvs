#pragma once

#include <QWidget>

#include "buffers/rgb24/rgb24Buffer.h"
#include "distortionParameters.h"
#include "math/vector/vector3d.h"
#include "buffers/displacementBuffer.h"
#include "segmentation/segmentator.h"
#include "widgets/observationListModel.h"

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
    void setBuffer(RGB24Buffer *buffer);

private:
    /* Images */
    RGB24Buffer         *mBufferInput;
    RGB24Buffer         *mBufferWithCorners;
    QList<QLine>         mCorrectionVectors;
    ObservationList      mObservationList;
    ObservationListModel mObservationListModel;

    //vector<PointObservation> mCorrectionMap;

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
    //RadialCorrection mLinesRadialCoorection;
    QSharedPointer<DisplacementBuffer> mDistortionCorrectTransform;

    /* Some data for semiautomatic corner detection*/
    Segmentator<CornerSegmentator, CornerSegment>::SegmentationResult *mResult;

    void addPointPair(const Vector3dd &spacePoint, const Vector2dd &imagePoint);

private slots:
//    void tryAddPointToPolygon(int toolID, QPointF const &point);
    void tryAddPoint(int toolID, QPointF const &point);
    void initExistingPoint(int toolID, QPointF const &point);
    void setParams();
    void initTransform();
    void detectCorners();
    void detectCheckerboard();
    void resetParameters();
    void showBufferChanged();
    void updateScore();

signals:
    void recalculationFinished(QSharedPointer<DisplacementBuffer> result);

private:
    Ui::DistortionWidget *mUi;
};
