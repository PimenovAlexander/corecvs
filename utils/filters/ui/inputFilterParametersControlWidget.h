#pragma once
#include <QWidget>
#include "generated/inputFilterParameters.h"
#include "ui_inputFilterParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"


namespace Ui {
    class InputFilterParametersControlWidget;
}

class InputFilterParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit InputFilterParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~InputFilterParametersControlWidget();

    InputFilterParameters* createParameters() const;
    void setParameters(const InputFilterParameters &input);
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
    Ui_InputFilterParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

