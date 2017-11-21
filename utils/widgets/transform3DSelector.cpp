#include "transform3DSelector.h"
#include "core/math/matrix/matrix33.h"
#include "core/math/mathUtils.h"
#include "core/math/vector/vector3d.h"

using corecvs::Matrix33;
using corecvs::degToRad;
using corecvs::Vector3dd;

Transform3DSelector::Transform3DSelector(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
}


Matrix44 Transform3DSelector::getTransform()
{
    bool scaleLocked = ui.lockScaleToolButton->isChecked();
    Matrix33 A =
        Matrix33::RotationZ(degToRad(ui.rotateZSpinBox->value())) *
        Matrix33::RotationY(degToRad(ui.rotateYSpinBox->value())) *
        Matrix33::RotationX(degToRad(ui.rotateXSpinBox->value())) *
        Matrix33::Scale3(Vector3dd(
            ui.scaleXSpinBox->value(),
            scaleLocked ? ui.scaleXSpinBox->value() : ui.scaleYSpinBox->value(),
            scaleLocked ? ui.scaleXSpinBox->value() : ui.scaleZSpinBox->value()
        ));


    return Matrix44(A, Vector3dd(
            ui.shiftXSpinBox->value(),
            ui.shiftYSpinBox->value(),
            ui.shiftZSpinBox->value()
    ));
}


void Transform3DSelector::parametersChangedSlot( void )
{
    emit parametersChanged();
}

Transform3DSelector::~Transform3DSelector()
{

}
