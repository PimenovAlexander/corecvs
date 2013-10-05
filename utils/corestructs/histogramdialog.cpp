#include "histogramdialog.h"

HistogramDialog::HistogramDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);
//    connect(this,SIGNAL(notifyBufferChange(G12Buffer *)), ui.widget, SLOT(notifyBufferChange(G12Buffer *)));
    connect(ui.marginBox, SIGNAL(stateChanged(int)), this, SLOT(doUpdate()));
    isInternal = false;
    histogram = NULL;
}

void HistogramDialog::notifyBufferChangeSlot(G12Buffer *buffer)
{
    if (isInternal && histogram != NULL)
    {
        delete histogram;
    }

    histogram = NULL;

    if (buffer != NULL)
    {
        isInternal = true;
        histogram = new Histogram(buffer);
    }
    doUpdate();
}

void HistogramDialog::notifyHistogramChangeSlot(Histogram *_histogram)
{
    if (isInternal && histogram != NULL)
    {
        delete histogram;
    }

    histogram = _histogram;
    isInternal = false;

    doUpdate();
}

void HistogramDialog::doUpdate()
{
    ui.widget->notifyHistogramChange(histogram, ui.marginBox->isChecked());
    if (histogram != NULL)
    {
        ui.maxLabel->setText(QString("%1").arg(histogram->getMaximum()));
        ui.statistics->setText(QString("L-%1 (%2%) H-%3(%4%)").
                arg(histogram->getUnderExpo()).arg(histogram->getUnderExpoRel() * 100.0).
                arg(histogram->getOverExpo()).arg(histogram->getOverExpoRel() * 100.0));
    }
}

HistogramDialog::~HistogramDialog()
{

}
