#pragma once
#include <QWidget>
#include "core/xml/generated/binarizeParameters.h"
#include "ui_binarizeParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"

namespace Ui {
    class BinarizeParametersControlWidget;
}

class BinarizeParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit BinarizeParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~BinarizeParametersControlWidget();

    BinarizeParameters* createParameters() const;

    void getParameters(BinarizeParameters& params) const;
    void setParameters(const BinarizeParameters &input);

    virtual BaseReflectionStatic *createParametersVirtual() const
    {
        return createParameters();
    }

    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    virtual void setParametersVirtual(void *input);

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();

private:
    Ui_BinarizeParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

