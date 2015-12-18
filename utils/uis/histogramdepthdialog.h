#pragma once

#include <QtGui/QDialog>
#include <QtCore/QMap>
#include <iostream>

#include "histogram.h"

namespace Ui {
    class HistogramDepthDialog;
}

using namespace corecvs;

class HistogramDepthDialog : public QDialog
{
    Q_OBJECT

public:
    HistogramDepthDialog(QWidget *parent = 0);
    ~HistogramDepthDialog();
    QMap<int, double> getDepthMap() const;  // возвращает копию на момент вызова
    void setZones(int zoneStart, int zoneEnd, int preZoneStart, int clickZoneEnd);

signals:
    void histogramClicked(const int value);
    void preZoneChanged(int zone);
    void zoneStartChanged(int zone);
    void zoneEndChanged(int zone);
    void clickZoneEndChanged(int zone);

public slots:
    virtual void setDepthesSlot(/*const*/ QSharedPointer<QMap<int, int> > /*&*/depthData);

protected:
    void changeEvent(QEvent *e);
    void redrawHistogram();

private:
    Ui::HistogramDepthDialog *mUi;
    QSharedPointer<QMap<int, int> > mDepthData;
    QMap<int, double> mDepthMap;

    Histogram *mCoreHistogram;

private slots:
    virtual void histogramClickSlot(const int value);

    void on_clearAllDepthesPushButton_clicked();
    void on_acceptDepthPushButton_clicked();
    void on_setPreZonePushButton_toggled(bool checked);
    void on_setZoneStartPushButton_toggled(bool checked);
    void on_setZoneEndPushButton_toggled(bool checked);
    void on_setClickZoneEndPushButton_toggled(bool checked);
    void on_nonePushButton_toggled(bool checked);
    void on_zoomInPushButton_clicked();
    void on_zoomOutPushButton_clicked();
};
