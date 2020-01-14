#include "statisticsDialog.h"

StatisticsDialog::StatisticsDialog(QWidget *parent)
    : ViAreaWidget(parent),
      statsAdded(0),
      captureAdded(0)

{
    ui.setupUi(this);
    redrawCaptureStats();
    redrawStats();
}

QString StatisticsDialog::printMs(uint64_t ms)
{
    return QString("%1 | %2 us").arg(ms / 1000).arg(ms % 1000, 3, 10, QChar('0'));
}

void StatisticsDialog::addStats(Statistics &newStats)
{
    statsAccum.addStatistics(newStats);
    redrawStats();
}

void StatisticsDialog::addCaptureStats(CaptureStatistics capture)
{
    captureCurrent = capture;
    captureAccum += capture;
    captureAdded++;
    redrawCaptureStats();
}


void StatisticsDialog::redrawCaptureStats()
{
   QString stats;
   for (int i = 0; i < CaptureStatistics::MAX_TIME_ID; i++)
   {
       double meanValue = 0.0;
       if (captureAdded != 0 ) {
           meanValue = captureAccum.values[i] / captureAdded;
       }

       stats += QString("%1 : %2 us : %3 ms : %4 us \n")
               .arg(CaptureStatistics::names[i], -20)
               .arg(captureCurrent.values[i], 7)
               .arg((captureCurrent.values[i] + 500) / 1000, 7)
               .arg(meanValue , 7);
   }


   stats += QString("%1 : %2 Kb\n")
            .arg(CaptureStatistics::names[CaptureStatistics::DATA_SIZE], -20)
            .arg(captureCurrent.values[CaptureStatistics::DATA_SIZE] / 1024, 7);

   uint64_t interframe = 0.0;
   if (captureAdded != 0) {
       interframe = captureAccum.values[CaptureStatistics::INTERFRAME_DELAY] / captureAdded;
   }

   if (interframe != 0)
   {
       double fps = 1000000.0 / interframe;
       stats += QString("Stereo FPS : %1\n").arg(fps);//.arg(1000000.0 / interframe, 5, 'f', 1);
   }

   double meanSkipped = 0.0;
   if (captureAdded != 0 ) {
       meanSkipped = (double)captureAccum.framesSkipped / captureAdded;
   }
   stats += QString("Frames skipped : %1%\n")
           .arg(meanSkipped * 100.0, 0, 'f', 2);
   stats += QString("Triggers skipped : %1%\n").arg(captureAccum.triggerSkipped);


   stats += QString("Temperature : %1C   %2C \n").arg(captureAccum.temperature[0], 0, 'f', 2).arg(captureAccum.temperature[1], 0, 'f', 2);

   ui.captureStats->setText(stats);
}

void StatisticsDialog::redrawStats()
{
    QString stats = statsAccum.printForQT();
    ui.flowStats->setText(stats);
}

void StatisticsDialog::reset()
{
    cout << "Resetting statistics" << endl;
    statsAccum.reset();
    statsAdded = 0;
    captureAccum = CaptureStatistics();
    captureCurrent = CaptureStatistics();
    captureAdded = 0;
    redrawCaptureStats();
    redrawStats();
}

StatisticsDialog::~StatisticsDialog()
{
}
