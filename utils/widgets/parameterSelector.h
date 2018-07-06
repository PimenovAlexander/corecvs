#ifndef PARAMETERSELECTOR_H
#define PARAMETERSELECTOR_H

#include <vector>

#include "parameterEditorWidget.h"
#include "ui_parameterSelector.h"

class ParameterSelector : public ParameterEditorWidget
{
    Q_OBJECT

public:
    ParameterSelector(QWidget *parent = 0);
   ~ParameterSelector();

    virtual double minimum (void) override;
    virtual double maximum (void) override;
    virtual double value   (void) override;

    virtual void setValue  (double value) override;
  /*  virtual void setMinimum(double value);
    virtual void setMaximum(double value);
    virtual void setStep   (double value);*/

    virtual void pushOption(QString name, int value);

    virtual void setAutoSupported(bool value) override;

    virtual void stepUp  () override;
    virtual void stepDown() override;

    virtual void setName(QString name) override;

signals:
    void valueChanged(int value);
    void autoToggled();
    void resetPressed();

public slots:
    void currentIndexChanged(int value);

private:
    Ui::ParameterSelectorClass ui;
    std::vector<int>           mIndexToValue;
};

#endif // PARAMETERSELECTOR_H
