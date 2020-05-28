#pragma once
#include <QWidget>
#include "core/xml/generated/operationParameters.h"
#include "ui_operationParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"
#include "filters/filterParametersControlWidgetBase.h"


namespace Ui {
    class OperationParametersControlWidget;
}

class OperationParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit OperationParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~OperationParametersControlWidget();

    OperationParameters* createParameters() const;
    void getParameters(OperationParameters &params) const;
    void setParameters(const OperationParameters &input);
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
    Ui_OperationParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

