
#pragma once
#include <QWidget>
#include <QLineEdit>
#include "generatedParameters/scanner.h"
#include "ui_scannerControlWidget.h"
#include "parametersControlWidgetBase.h"

namespace Ui {
    class ScannerControlWidget;
}

class ScannerControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ScannerControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~ScannerControlWidget();

    Scanner* createParameters() const;

    void getParameters(Scanner &params) const;
    void setParameters(const Scanner &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::ScannerControlWidget *ui()
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
    Ui::ScannerControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


