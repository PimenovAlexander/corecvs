#include <math.h>
#include <stdio.h>

#include "exponentialSlider.h"
#include "mathUtils.h"

ExponentialSlider::ExponentialSlider(QWidget *parent)
    : QWidget(parent)
    , sliderSteps(100)
    , sliderExp(1.05)
{
    ui.setupUi(this);
    setMaxZoom(100);
}

ExponentialSlider::~ExponentialSlider()
{

}

void ExponentialSlider::setMaxZoom(double value, double sliderSteps)
{
    ui.expSpinBox->setMaximum(      value);
    ui.expSpinBox->setMinimum(1.0 / value);

    ui.expSlider->setMaximum( sliderSteps);
    ui.expSlider->setMinimum(-sliderSteps);

    double power = log(value);
    double powerStep = power / sliderSteps;
    sliderExp = powerStep;
}

double ExponentialSlider::minimum (void)
{
    return ui.expSpinBox->minimum();
}

double ExponentialSlider::maximum (void)
{
    return ui.expSpinBox->maximum();
}

double ExponentialSlider::value()
{
    if (ui.expSpinBox->value() == 0.0) {
        qDebug("ExponentialSlider::value(): Oops crash possible returing zero\n");
    }
    return ui.expSpinBox->value();
}

void ExponentialSlider::setValue(double value)
{
    ui.expSpinBox->setValue(value);
}

void ExponentialSlider::stepUp  ()
{
    ui.expSlider->setValue(ui.expSlider->value() + 1);
}

void ExponentialSlider::stepDown()
{
    ui.expSlider->setValue(ui.expSlider->value() - 1);
}

void ExponentialSlider::expTextChanged   (double value)
{
    double loge = log(value);
    double val  = loge / sliderExp;

    bool blocked = ui.expSlider->blockSignals(true);
    ui.expSlider->setValue(val);
    ui.expSlider->blockSignals(blocked);

    emit valueChanged(value);
}

void ExponentialSlider::expSliderChanged (int val)
{
    double value = pow(M_E, sliderExp * val);
    bool blocked = ui.expSpinBox->blockSignals(true);
    ui.expSpinBox->setValue(value);
    ui.expSpinBox->blockSignals(blocked);

    emit valueChanged(value);
}

