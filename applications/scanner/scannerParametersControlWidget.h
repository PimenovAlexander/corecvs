
#pragma once
#include <QWidget>
#include <QLineEdit>
#include "generatedParameters/scannerParameters.h"
#include "ui_scannerParametersControlWidget.h"
#include "parametersControlWidgetBase.h"

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

private:
    Ui::ScannerParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


