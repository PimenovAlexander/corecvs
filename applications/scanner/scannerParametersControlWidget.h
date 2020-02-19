
#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include "generatedParameters/scannerParameters.h"
#include "ui_scannerParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"
#include "scannerThread.h"

namespace Ui {
    class ScannerParametersControlWidget;
}

class ScannerParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ScannerParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~ScannerParametersControlWidget();

    ScannerParameters* createParameters() const;

    void getParameters(ScannerParameters &params) const;
    void setParameters(const ScannerParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::ScannerParametersControlWidget *ui()
    {
        return mUi;
    }

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();

protected:
    Ui::ScannerParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

class ScannerParametersControlWidgetAdv : public ScannerParametersControlWidget
{
    Q_OBJECT

public:
    QPushButton *startButton = NULL;

    explicit ScannerParametersControlWidgetAdv(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString()) :
        ScannerParametersControlWidget(parent, autoInit, rootPath)
    {
        startButton = new QPushButton(this);
        startButton->setText("Start scan");
        mUi->gridLayout->addWidget(startButton);
    }


};

