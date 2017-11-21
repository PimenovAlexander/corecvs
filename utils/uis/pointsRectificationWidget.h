#pragma once

#include <QWidget>
#include <QStandardItemModel>

#include "paintImageWidget.h"
#include "frames.h"
#include "core/rectification/correspondenceList.h"

namespace Ui {
class PointsRectificationWidget;
}

class PointsRectificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointsRectificationWidget(QWidget *parent = 0);
    ~PointsRectificationWidget();
    void setImage(RGB24Buffer *buffer, Frames::FrameSourceId id);
    void addPointPair(QPointF const &leftPoint, QPointF const &rightPoint);

private:
    Ui::PointsRectificationWidget *mUi;
    QStandardItemModel *mPointModel;
    static bool isNextRow(QModelIndex const &index1, QModelIndex const &index2);
    void initModel();
    void setNewPointsCoord(QPointF const &prevPoint, QPointF const &newPoint, bool isLeftImage);
    CorrespondenceList *mCorrespondencePoints;
    RGB24Buffer *mLeftBuffer;
    RGB24Buffer *mRightBuffer;

private slots:
    void deletePairs();
    void addPair();
    void editPointLeftImage (QPointF const &prevPoint, QPointF const &newPoint);
    void editPointRightImage(QPointF const &prevPoint, QPointF const &newPoint);
    void selectionChanged(QItemSelection const &selected, QItemSelection const &deselected);
    void initCorrespondencePoints();

    /* New style */
    //void addPoint();

    void updateRightZoom  (double zoom);
    void updateLeftZoom   (double zoom);
    void updateRightCenter(QPoint center);
    void updateLeftCenter (QPoint center);


signals:
    void readyCorrespondencePoints(CorrespondenceList *correspondencePoints, RGB24Buffer *leftBuffer, RGB24Buffer *rightBuffer);
};

