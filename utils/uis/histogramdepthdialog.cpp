#include <QtCore/QDebug>

#include "histogramdepthdialog.h"
#include "ui_histogramdepthdialog.h"

// TODO: It shall be equal to maximum possible disparity, which seems to be dependent
// on image resolution, so this value shall not be a constant at all.
int const histogramMargin = 640;

using namespace std;

HistogramDepthDialog::HistogramDepthDialog(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::HistogramDepthDialog),
    mCoreHistogram(NULL)
{
    mUi->setupUi(this);

    //qDebug() << "HistogramDepthDialog::HistogramDepthDialog( _ ) called";

    redrawHistogram();
    connect(mUi->corePlot, SIGNAL(histogramBarClicked(int)), this, SLOT(histogramClickSlot(int)));
    connect(mUi->corePlot, SIGNAL(preZoneChanged(int)), this, SIGNAL(preZoneChanged(int)));
    connect(mUi->corePlot, SIGNAL(zoneStartChanged(int)), this, SIGNAL(zoneStartChanged(int)));
    connect(mUi->corePlot, SIGNAL(zoneEndChanged(int)), this, SIGNAL(zoneEndChanged(int)));
    connect(mUi->corePlot, SIGNAL(clickZoneEndChanged(int)), this, SIGNAL(clickZoneEndChanged(int)));
}

void HistogramDepthDialog::redrawHistogram()
{
//    qDebug() << "HistogramDepthDialog::redrawHistogram(): " << mCoreHistogram;

    mUi->corePlot->show();
    // Use signal here to serialize
    if (mCoreHistogram != NULL)
    {
        mUi->corePlot->notifyHistogramChange(mCoreHistogram, true);
    }
}

void HistogramDepthDialog::setDepthesSlot(/*const*/ QSharedPointer<QMap<int, int> > /*&*/depthData)
{
    if (depthData.isNull() || depthData->count() == 0) {
        return;
    }
//    qDebug("HistogramDepthDialog::setDepthesSlot() called");

    mDepthData = depthData;
    QMap<int, int> &qMap = *mDepthData.data();
    Histogram *histogramToDelete = mCoreHistogram;
    int minX = (qMap.begin()    ).key();
    int maxX = (qMap.end()   - 1).key();

    mCoreHistogram = new Histogram(-histogramMargin, histogramMargin);
    for (int i = max(minX, mCoreHistogram->min); i < min(maxX, mCoreHistogram->max); i++)
    {
        mCoreHistogram->set(i, qMap[i]);
    }
    delete_safe(histogramToDelete);
    redrawHistogram();

}

void HistogramDepthDialog::histogramClickSlot(const int value)
{
    if (mDepthData)
    {
        mUi->label->setText("Selected depth: " + QString::number(value)
                       + ". Number of points: " + QString::number(mDepthData.data()->operator [] (value)));
        mUi->depthSpinBox->setValue(value);
        //ui->realDepthSpinBox->setValue(0);
        emit(histogramClicked(value));
    }
}

HistogramDepthDialog::~HistogramDepthDialog()
{
    delete_safe(mCoreHistogram);
    delete_safe(mUi);
}

void HistogramDepthDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        mUi->retranslateUi(this);
        break;
    default:
        break;
    }
}

void HistogramDepthDialog::on_acceptDepthPushButton_clicked()
{
    if (mDepthData.isNull())
        return;

    mDepthMap[mUi->depthSpinBox->value()] = mUi->realDepthSpinBox->value();
    mUi->learnedEntriesLabel2->setText(QString::number(mDepthMap.count()));
}

void HistogramDepthDialog::on_clearAllDepthesPushButton_clicked()
{
    mDepthMap.clear();
    mUi->learnedEntriesLabel2->setText("0");
}

QMap<int, double> HistogramDepthDialog::getDepthMap() const
{
    return mDepthMap;
}

void HistogramDepthDialog::setZones(int zoneStart, int zoneEnd, int preZoneStart, int clickZoneEnd)
{
    if (preZoneStart > zoneStart || zoneStart > zoneEnd || zoneEnd > clickZoneEnd)
        return;
    mUi->corePlot->setZones(zoneStart, zoneEnd, preZoneStart, clickZoneEnd);
}

void HistogramDepthDialog::on_setPreZonePushButton_toggled(bool checked)
{
    if (checked)
        mUi->corePlot->setZoneSettingState(HistogramWidget::preZone);
}

void HistogramDepthDialog::on_setZoneStartPushButton_toggled(bool checked)
{
    if (checked)
        mUi->corePlot->setZoneSettingState(HistogramWidget::zoneStart);
}

void HistogramDepthDialog::on_setZoneEndPushButton_toggled(bool checked)
{
    if (checked)
        mUi->corePlot->setZoneSettingState(HistogramWidget::zoneEnd);
}

void HistogramDepthDialog::on_setClickZoneEndPushButton_toggled(bool checked)
{
    if (checked)
        mUi->corePlot->setZoneSettingState(HistogramWidget::clickEnd);
}

void HistogramDepthDialog::on_nonePushButton_toggled(bool checked)
{
    if (checked)
        mUi->corePlot->setZoneSettingState(HistogramWidget::none);
}

void HistogramDepthDialog::on_zoomInPushButton_clicked()
{
    mUi->corePlot->zoomIn();
}

void HistogramDepthDialog::on_zoomOutPushButton_clicked()
{
    mUi->corePlot->zoomOut();
}
