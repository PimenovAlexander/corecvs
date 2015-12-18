#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QtGui/QWidget>

#include "global.h"

#include "ui_statisticsDialog.h"
#include "qtStatisticsCollector.h"
#include "imageCaptureInterface.h"
#include "viAreaWidget.h"

class StatisticsDialog : public ViAreaWidget
{
    Q_OBJECT

public slots:
    void addCaptureStats(CaptureStatistics capture);
    void addStats(Statistics &flow);
    void reset();

public:
    StatisticsDialog(QWidget *parent = 0);
    ~StatisticsDialog();

    void redrawCaptureStats();
    void redrawStats();

    QString printMs(uint64_t ms);

    QtStatisticsCollector statsAccum;
private:
    Ui_StatisticsDialogClass ui;
    CaptureStatistics captureAccum;
    CaptureStatistics captureCurrent;

    int statsAdded;
    int captureAdded;
};

#endif // STATISTICSDIALOG_H
