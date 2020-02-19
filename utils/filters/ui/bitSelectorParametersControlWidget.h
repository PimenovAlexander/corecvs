#pragma once

#include "ui_bitSelectorParametersControlWidget.h"
#include "core/xml/generated/bitSelectorParameters.h"
#include "filters/filterParametersControlWidgetBase.h"

namespace Ui {
    class BitSelectorParametersControlWidget;
}

class BitSelectorParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit BitSelectorParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    virtual ~BitSelectorParametersControlWidget();
    BitSelectorParameters* createParameters() const;
    void getParameters(BitSelectorParameters &params) const;


    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return createParameters();
    }

    void setParameters(const BitSelectorParameters &input);
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
    Ui_BitSelectorParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


