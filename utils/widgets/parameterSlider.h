#ifndef PARAMETERSLIDER_H
#define PARAMETERSLIDER_H

#include <QtGui/QWidget>
#include "ui_parameterSlider.h"
#include "parameterEditorWidget.h"

class ParameterSlider : public ParameterEditorWidget
{
    Q_OBJECT

public:
    ParameterSlider(QWidget *parent = 0);
    ~ParameterSlider();

    virtual double minimum (void);
    virtual double maximum (void);
    virtual double value   (void);

    virtual void setValue  (double value);
    virtual void setMinimum(double value);
    virtual void setMaximum(double value);
    virtual void setStep   (double value);
    virtual void setAutoSupported(bool value);

    virtual void stepUp  ();
    virtual void stepDown();

    virtual void setName (QString name);

public slots:
    virtual void textChanged   (int value);
    virtual void sliderChanged (int value);

signals:
    void valueChanged(int value);
    void autoToggled();
    void resetPressed();

private:
    Ui::ParameterSliderClass ui;
};

#endif // PARAMETERSLIDER_H
