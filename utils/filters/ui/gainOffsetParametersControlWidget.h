#pragma once


#include "filterParametersControlWidgetBase.h"
#include "generated/gainOffsetParameters.h"
#include "ui_gainOffsetParametersControlWidget.h"

namespace Ui {
    class GainOffsetParametersControlWidget;
}

class GainOffsetParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit GainOffsetParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    virtual ~GainOffsetParametersControlWidget();
    GainOffsetParameters* createParameters() const;
    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return createParameters();
    }

    void getParameters(GainOffsetParameters &params) const;
    void setParameters(const GainOffsetParameters &input);
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
    Ui_GainOffsetParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


