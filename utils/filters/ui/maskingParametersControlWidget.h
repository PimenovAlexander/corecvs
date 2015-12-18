#pragma once
#include <QWidget>
#include "generated/maskingParameters.h"
#include "ui_maskingParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"

namespace Ui {
    class MaskingParametersControlWidget;
}

class MaskingParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit MaskingParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~MaskingParametersControlWidget();

    MaskingParameters* createParameters() const;
    void getParameters(MaskingParameters &params) const;
    void setParameters(const MaskingParameters &input);
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
    Ui_MaskingParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
