#include <math.h>
#include <stdio.h>

#include "parameterSlider.h"
#include "mathUtils.h"

ParameterSlider::ParameterSlider(QWidget *parent)
    : ParameterEditorWidget(parent)
{
    ui.setupUi(this);
    ui.autoBox->hide();
    connect(ui.resetButton, SIGNAL(released()), this, SIGNAL(resetPressed()));
}

ParameterSlider::~ParameterSlider()
{

}


void ParameterSlider::setName (QString name)
{
    ui.label->setText(name);
}

void ParameterSlider::setAutoSupported(bool value)
{
    ui.autoBox->setEnabled(value);
}

double ParameterSlider::minimum (void)
{
    return ui.spinBox->minimum();
}

double ParameterSlider::maximum (void)
{
    return ui.spinBox->maximum();
}

double ParameterSlider::value()
{
    return ui.spinBox->value();
}

void ParameterSlider::setValue(double value)
{
    ui.spinBox->setValue(value);
}

void ParameterSlider::setMinimum(double value)
{
    ui.spinBox->setMinimum(value);
    ui.slider->setMinimum(value);
    ui.minLabel->setText(QString::number(value));
}

void ParameterSlider::setMaximum(double value)
{
    ui.spinBox->setMaximum(value);
    ui.slider->setMaximum(value);
    ui.maxLabel->setText(QString::number(value));
}

void ParameterSlider::setStep (double value)
{
    ui.spinBox->setSingleStep(value);
    ui.slider->setSingleStep(value);
}

void ParameterSlider::stepUp  ()
{
    ui.slider->setValue(ui.slider->value() + 1);
}

void ParameterSlider::stepDown()
{
    ui.slider->setValue(ui.slider->value() - 1);
}

void ParameterSlider::textChanged   (int value)
{
    bool blocked = ui.slider->blockSignals(true);
    ui.slider->setValue(value);
    ui.slider->blockSignals(blocked);

    emit valueChanged(value);
}

void ParameterSlider::sliderChanged (int value)
{
    bool blocked = ui.spinBox->blockSignals(true);
    ui.spinBox->setValue(value);
    ui.spinBox->blockSignals(blocked);

    emit valueChanged(value);
}

