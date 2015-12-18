#pragma once

#include "generated/outputFilterParameters.h"
#include "ui_outputFilterParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"


namespace Ui {
    class OutputFilterParametersControlWidget;
}

class OutputFilterParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit OutputFilterParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~OutputFilterParametersControlWidget();

    OutputFilterParameters* createParameters() const;
    void getParameters(OutputFilterParameters &params) const;
    void setParameters(const OutputFilterParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return createParameters();
    }

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
    Ui_OutputFilterParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

