#pragma once
#include <QtGui/QWidget>
#include "generated/cannyParameters.h"
#include "ui_cannyParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"


namespace Ui {
    class CannyParametersControlWidget;
}

class CannyParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit CannyParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~CannyParametersControlWidget();

    CannyParameters* createParameters() const;
    void setParameters(const CannyParameters &input);
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
    Ui_CannyParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

