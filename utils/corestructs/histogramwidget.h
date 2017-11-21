#pragma once

#include <QWidget>
#include <QtGui/QMouseEvent>
#include "core/buffers/histogram/histogram.h"

using namespace corecvs;

namespace Ui {
class HistogramWidgetClass;
}

class HistogramWidget : public QWidget
{
    Q_OBJECT

public slots:
    void notifyHistogramChange(Histogram *histogram, bool useMargin);
    void zoomIn();
    void zoomOut();

public:
    enum ZoneSettingState {
        none
        , preZone
        , zoneStart
        , zoneEnd
        , clickEnd
    };

    HistogramWidget(QWidget *parent = 0);
    ~HistogramWidget();

    void setZoneSettingState(ZoneSettingState state);
    void setZones(int zoneStart, int zoneEnd, int preZoneStart, int clickZoneEnd);

signals:
    void histogramBarClicked(int bar);
    void preZoneChanged(int zone);
    void zoneStartChanged(int zone);
    void zoneEndChanged(int zone);
    void clickZoneEndChanged(int zone);

private:
    int mSelectedH;
    Ui::HistogramWidgetClass *mUi;
    bool mUseMargin;
    Histogram *mHistogram;

    double mZoneStart;
    double mZoneEnd;
    double mPreZoneStart;
    double mClickZoneEnd;
    ZoneSettingState mZoneSettingState;
    bool mSettingZone;

    double mZoomFactor;
    double mFrameLeftBorder;  // In histogram absolute coordinates
    QPointF mPrevMousePos;

    void drawGrid(QPainter &painter, int scaleX, int scaleY);
    bool getBarByX(int x, int *bar);

    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void setZone(int zoneBorder);
    void recalculateFrame();

    double toAbsolute(double const &x) const;
    double toRelative(double const &x) const;

};
