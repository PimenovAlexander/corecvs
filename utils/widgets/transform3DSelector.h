#ifndef TRANSFORM3DSELECTOR_H
#define TRANSFORM3DSELECTOR_H



#include "matrix44.h"
#include "ui_transform3DSelector.h"
#include <QWidget>

using corecvs::Matrix44;

class Transform3DSelector : public QWidget
{
    Q_OBJECT

public:
    Transform3DSelector(QWidget *parent = 0);
    ~Transform3DSelector();

    Matrix44 getTransform();

    /* not that nice, but is simple enough*/
template <class Visitor>
	void accept(Visitor &visitor)
	{
		bool scaleLocked = ui.lockScaleToolButton->isChecked();
		double rotateZ = ui.rotateZSpinBox->value();
		double rotateY = ui.rotateYSpinBox->value();
		double rotateX = ui.rotateXSpinBox->value();

		double scaleZ = ui.scaleZSpinBox->value();
		double scaleY = ui.scaleYSpinBox->value();
		double scaleX = ui.scaleXSpinBox->value();

		double shiftZ = ui.shiftZSpinBox->value();
		double shiftY = ui.shiftYSpinBox->value();
		double shiftX = ui.shiftXSpinBox->value();

		visitor.visit(scaleLocked, true, "scaleLocked");
		visitor.visit(rotateZ, 0.0, "rotateZ");
		visitor.visit(rotateY, 0.0, "rotateY");
		visitor.visit(rotateX, 0.0, "rotateX");

		visitor.visit(scaleZ, 1.0, "scaleZ");
		visitor.visit(scaleY, 1.0, "scaleY");
		visitor.visit(scaleX, 1.0, "scaleX");

		visitor.visit(shiftZ, 0.0, "shiftZ");
		visitor.visit(shiftY, 0.0, "shiftY");
		visitor.visit(shiftX, 0.0, "shiftX");

		ui.lockScaleToolButton->setChecked(scaleLocked);
		ui.rotateZSpinBox->setValue(rotateZ);
		ui.rotateYSpinBox->setValue(rotateY);
		ui.rotateXSpinBox->setValue(rotateX);

		ui.scaleZSpinBox->setValue(scaleZ);
		ui.scaleYSpinBox->setValue(scaleY);
		ui.scaleXSpinBox->setValue(scaleX);

		ui.shiftZSpinBox->setValue(shiftZ);
		ui.shiftYSpinBox->setValue(shiftY);
		ui.shiftXSpinBox->setValue(shiftX);
	}


public slots:
    void parametersChangedSlot( void );

signals:
    void parametersChanged(void );


private:
    Ui::Transform3DSelectorClass ui;
};

#endif // TRANSFORM3DSELECTOR_H
