#pragma once
#include <QWidget>
#include "core/xml/generated/axisAlignedBoxParameters.h"
#include "ui_axisAlignedBoxParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class AxisAlignedBoxParametersControlWidget;
}

class AxisAlignedBoxParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit AxisAlignedBoxParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~AxisAlignedBoxParametersControlWidget();

    AxisAlignedBoxParameters* createParameters() const;
    void getParameters(AxisAlignedBoxParameters &param) const;
    void setParameters(const AxisAlignedBoxParameters &input);
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
    Ui_AxisAlignedBoxParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

