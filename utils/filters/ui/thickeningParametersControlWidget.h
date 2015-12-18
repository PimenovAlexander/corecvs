#pragma once
#include <QWidget>
#include "generated/thickeningParameters.h"
#include "ui_thickeningParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"

namespace Ui {
    class ThickeningParametersControlWidget;
}

class ThickeningParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit ThickeningParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~ThickeningParametersControlWidget();

    ThickeningParameters* createParameters() const;

    void getParameters(ThickeningParameters& params) const;
    void setParameters(const ThickeningParameters &input);
    void setParametersVirtual(void *input);

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
    Ui_ThickeningParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
