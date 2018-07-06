#ifndef PARAMETERSLIDER_H
#define PARAMETERSLIDER_H

#include "parameterEditorWidget.h"
#include "ui_parameterSlider.h"

class ParameterSlider : public ParameterEditorWidget
{
    Q_OBJECT

public:
    ParameterSlider(QWidget *parent = 0);
    ~ParameterSlider();

    virtual double minimum (void) override;
    virtual double maximum (void) override;
    virtual double value   (void) override;

    virtual void setValue  (double value) override;
    virtual void setMinimum(double value);
    virtual void setMaximum(double value);
    virtual void setStep   (double value);

    virtual void setAutoSupported(bool value) override;

    virtual void stepUp  () override;
    virtual void stepDown() override;

    virtual void setName (QString name) override;

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
