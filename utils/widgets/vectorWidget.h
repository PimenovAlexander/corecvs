#ifndef VECTORWIDGET_H
#define VECTORWIDGET_H

#include <QWidget>
#include <QDoubleSpinBox>
#include "parametersControlWidgetBase.h"


namespace Ui {
class VectorWidget;
}

class VectorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit VectorWidget(QWidget *parent = 0);
    virtual ~VectorWidget();

    ParametersControlWidgetBaseFabric *mFabric = NULL;

public slots:
    void buttonsSide(bool flag);
    void addEntry();
    void removeEntry();

signals:

private:
    Ui::VectorWidget *ui;
};



class DoubleVectorWidget : public QWidget
{
      Q_OBJECT
public:
    explicit DoubleVectorWidget(QWidget *parent = 0);
    virtual ~DoubleVectorWidget();

    double mMinimum;
    double mMaximum;
    int mDecimals;
    double mSingleStep;


    Q_PROPERTY(double maximum READ maximum WRITE setMaximum);
    Q_PROPERTY(double minimum READ minimum WRITE setMinimum);

    Q_PROPERTY(int    decimals   READ decimals WRITE setDecimals);
    Q_PROPERTY(double singleStep READ singleStep WRITE setSingleStep);


public slots:
    void addEntry();
    void removeEntry();
    void resize(int size);

    void setMaximum(double value);
    void setMinimum(double value);

    void setDecimals  (int value);
    void setSingleStep(double value);

public:
    double maximum();
    double minimum();
    int decimals();
    double singleStep();

signals:
    void valueChanged();

public:
    vector<double> value();
    void setValue(const vector<double> &value);

    /**/



private:
    Ui::VectorWidget *ui;
};

#endif // VECTORWIDGET_H
