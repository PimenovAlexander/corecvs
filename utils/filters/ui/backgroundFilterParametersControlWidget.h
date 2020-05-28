#pragma once
#include <QWidget>
#include "core/xml/generated/backgroundFilterParameters.h"
#include "ui_backgroundFilterParametersControlWidget.h"
#include "filters/filterParametersControlWidgetBase.h"

namespace Ui {
    class BackgroundFilterParametersControlWidget;
}

class BackgroundFilterParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit BackgroundFilterParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~BackgroundFilterParametersControlWidget();

    BackgroundFilterParameters* createParameters() const;

    void getParameters(BackgroundFilterParameters& params) const;
    void setParameters(const BackgroundFilterParameters &input);
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
    Ui_BackgroundFilterParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

