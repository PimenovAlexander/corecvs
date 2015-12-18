#pragma once
#include <QWidget>
#include "generated/checkerboardDetectionParameters.h"
#include "ui_checkerboardDetectionParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class CheckerboardDetectionParametersControlWidget;
}

class CheckerboardDetectionParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit CheckerboardDetectionParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~CheckerboardDetectionParametersControlWidget();

    CheckerboardDetectionParameters* createParameters() const;
    void getParameters(CheckerboardDetectionParameters &params) const;
    void setParameters(const CheckerboardDetectionParameters &input);
    virtual void setParametersVirtual(void *input);
    
    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);
    
public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui_CheckerboardDetectionParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

