#include "angleEditBox.h"
#include "qtHelper.h"
#include "core/math/mathUtils.h"

using corecvs::radToDeg;
using corecvs::degToRad;


AngleEditBox::AngleEditBox(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);

    connect(ui.radSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(updateDegreeAngles ()));
    connect(ui.degSpinBox,  SIGNAL(valueChanged(double)), this, SLOT(updateRagAngles ()));
    connect(ui.resetButton, SIGNAL(released()), this, SLOT(reset()));
}

void AngleEditBox::updateDegreeAngles (void)
{
    setValueBlocking(ui.degSpinBox, radToDeg(ui.radSpinBox->value()));
    emit valueChanged(ui.radSpinBox->value());
}

void AngleEditBox::updateRagAngles (void)
{
    setValueBlocking(ui.radSpinBox, degToRad(ui.degSpinBox->value()));
    emit valueChanged(ui.radSpinBox->value());
}

void AngleEditBox::reset()
{
    ui.radSpinBox->setValue(0.0);
}


AngleEditBox::~AngleEditBox()
{

}
