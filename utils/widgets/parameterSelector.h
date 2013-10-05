#ifndef PARAMETERSELECTOR_H
#define PARAMETERSELECTOR_H

#include <vector>

#include <QtGui/QWidget>

#include "ui_parameterSelector.h"
#include "parameterEditorWidget.h"

using std::vector;

class ParameterSelector : public ParameterEditorWidget
{
    Q_OBJECT

public:
    ParameterSelector(QWidget *parent = 0);
    ~ParameterSelector();

    virtual double minimum (void);
    virtual double maximum (void);
    virtual double value   (void);

    virtual void setValue  (double value);
  /*  virtual void setMinimum(double value);
    virtual void setMaximum(double value);
    virtual void setStep   (double value);*/
    virtual void pushOption(QString name, int value);
    virtual void setAutoSupported(bool value);


    virtual void stepUp  ();
    virtual void stepDown();

    virtual void setName (QString name);

signals:
    void valueChanged(int value);
    void autoToggled();
    void resetPressed();

public slots:
    void currentIndexChanged (int value);

private:
    void regenComboBox();

    Ui::ParameterSelectorClass ui;
    vector<int> mIndexToValue;

};

#endif // PARAMETERSELECTOR_H
