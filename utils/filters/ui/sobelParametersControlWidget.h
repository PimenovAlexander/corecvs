#pragma once
#include <QWidget>
#include "core/xml/generated/sobelParameters.h"
#include "ui_sobelParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"


namespace Ui {
    class SobelParametersControlWidget;
}

class SobelParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit SobelParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~SobelParametersControlWidget();

    SobelParameters* createParameters() const;

    void getParameters(SobelParameters &params) const;
    void setParameters(const SobelParameters &input);
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
    Ui_SobelParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

