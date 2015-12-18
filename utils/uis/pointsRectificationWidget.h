#pragma once

#include <QWidget>
#include <QStandardItemModel>

#include "paintImageWidget.h"
#include "frames.h"
#include "correspondenceList.h"

namespace Ui {
class PointsRectificationWidget;
}

class PointsRectificationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PointsRectificationWidget(QWidget *parent = 0);
    ~PointsRectificationWidget();
    void setImage(G12Buffer *buffer, Frames::FrameSourceId id);
    void addPointPair(QPointF const &leftPoint, QPointF const &rightPoint);

private:
    Ui::PointsRectificationWidget *mUi;
    QStandardItemModel *mPointModel;
    static bool isNextRow(QModelIndex const &index1, QModelIndex const &index2);
    void initModel();
    void setNewPointsCoord(QPointF const &prevPoint, QPointF const &newPoint, bool isLeftImage);
    CorrespondenceList *mCorrespondencePoints;
    G12Buffer *mLeftBuffer;
    G12Buffer *mRightBuffer;

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
    void readyCorrespondencePoints(CorrespondenceList *correspondencePoints, G12Buffer *leftBuffer, G12Buffer *rightBuffer);
};

