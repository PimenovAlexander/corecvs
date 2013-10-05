#include <math.h>


#include "gainoffset.h"
#include "commonMappers.h"

#define MAX_GAIN  G12BUFFER_MAX_VALUE
#define MIN_GAIN  (1 / 1000.0)

#define MAX_OFFSET (G12Buffer::BUFFER_MAX_VALUE)
#define MIN_OFFSET (-CG12Buffer::BUFFER_MAX_VALUE)

QString GainOffset::name = QString("Gain&Offset");

GainOffset::GainOffset(QWidget *parent)
    : BaseFilter(parent)
{
    ui.setupUi(this);

    gParams.offset = 0;
    ui.offsetEdit->setValue(gParams.offset);
    ui.offsetEdit  ->setMaximum( G12Buffer::BUFFER_MAX_VALUE);
    ui.offsetEdit  ->setMinimum(-G12Buffer::BUFFER_MAX_VALUE);
    ui.offsetSlider->setMaximum( G12Buffer::BUFFER_MAX_VALUE);
    ui.offsetSlider->setMinimum(-G12Buffer::BUFFER_MAX_VALUE);

    ui.offsetSlider->setValue(gParams.offset);
    connect(ui.offsetEdit, SIGNAL(valueChanged(double)), this, SLOT(updateOffsetSlider(double)));
    connect(ui.offsetSlider, SIGNAL(valueChanged(int)), this, SLOT(updateOffsetText(int)));

    gParams.gain = 1;

    ui.gainEdit->setValue(gParams.gain);
    ui.gainSlider->setMaximum(100);
    ui.gainSlider->setMinimum(-100);
    ui.gainSlider->setValue(0);
    connect(ui.gainEdit, SIGNAL(valueChanged(double)), this, SLOT(updateGainSlider(double)));
    connect(ui.gainSlider, SIGNAL(valueChanged(int)), this, SLOT(updateGainText(int)));

}

G12Buffer *GainOffset::filter (G12Buffer *input)
{
    G12Buffer *toReturn = new G12Buffer(input);
    GainOffsetMapper gom(gParams.gain, gParams.offset);
    toReturn->mapOperationElementwize<GainOffsetMapper>(gom);
    return toReturn;
}


void GainOffset::updateOffsetSlider (double value)
{
    bool blocked = ui.offsetSlider->blockSignals(true);
    ui.offsetSlider->setValue(value);
    ui.offsetSlider->blockSignals(blocked);
    gParams.offset = value;
    emit parametersChanged();
    //notifyRecalculate();
}

void GainOffset::updateOffsetText (int val)
{
    bool blocked = ui.offsetEdit->blockSignals(true);
    ui.offsetEdit->setValue(val);
    ui.offsetEdit->blockSignals(blocked);
    gParams.offset = val;
    emit parametersChanged();
    //notifyRecalculate();
}


void GainOffset::updateGainSlider (double value)
{
    double loge = log(value);
    double log1_05 = log(1.05);
    value = loge / log1_05;

    bool blocked = ui.gainSlider->blockSignals(true);
    ui.gainSlider->setValue(value);
    ui.gainSlider->blockSignals(blocked);

    gParams.gain = value;
    emit parametersChanged();
//    notifyRecalculate();
}

void GainOffset::updateGainText (int val)
{
    double value = pow(1.05, val);
    bool blocked = ui.gainEdit->blockSignals(true);
    ui.gainEdit->setValue(value);
    ui.gainEdit->blockSignals(blocked);
    gParams.gain = value;
    emit parametersChanged();
//    notifyRecalculate();
}


GainOffset::~GainOffset()
{

}
