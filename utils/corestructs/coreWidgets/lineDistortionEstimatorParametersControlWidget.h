#pragma once
#include <QWidget>
#include "core/xml/generated/lineDistortionEstimatorParameters.h"
#include "ui_lineDistortionEstimatorParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class LineDistortionEstimatorParametersControlWidget;
}

class LineDistortionEstimatorParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit LineDistortionEstimatorParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~LineDistortionEstimatorParametersControlWidget();

    LineDistortionEstimatorParameters* createParameters() const;
    void getParameters(LineDistortionEstimatorParameters &param) const;
    void setParameters(const LineDistortionEstimatorParameters &input);
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
    Ui_LineDistortionEstimatorParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

