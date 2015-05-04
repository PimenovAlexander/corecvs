#pragma once

#include <QWidget>
#include "ui_angleEditBox.h"

class AngleEditBox : public QWidget
{
    Q_OBJECT

public:
    AngleEditBox(QWidget *parent = 0);
    ~AngleEditBox();

    double value()
    {
        return ui.radSpinBox->value();
    }

    void setValue(double val)
    {
        ui.radSpinBox->setValue(val);
        updateDegreeAngles();
    }


public slots:
    void updateDegreeAngles ();
    void updateRagAngles ();
    void reset();
signals:

    void valueChanged(double);

private:
    Ui::AngleEditBoxClass ui;
};


