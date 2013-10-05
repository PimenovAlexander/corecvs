#ifndef EXPONENTIALSLIDER_H
#define EXPONENTIALSLIDER_H

#include <QtGui/QWidget>
#include "ui_exponentialSlider.h"

class ExponentialSlider : public QWidget
{
    Q_OBJECT

public:
    ExponentialSlider(QWidget *parent = 0);
    ~ExponentialSlider();

    void setMaxZoom(double value, double sliderSteps = 100);

    double minimum (void);
    double maximum (void);
    double value   (void);

    void setValue(double value);

    void stepUp  ();
    void stepDown();

public slots:
    void expTextChanged   (double value);
    void expSliderChanged (int val);

signals:
    void valueChanged(double value);

private:
    Ui::ExponentialSliderClass ui;
    int sliderSteps;
    double sliderExp;
};

#endif // EXPONENTIALSLIDER_H
