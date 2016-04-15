#pragma once
#include <QWidget>
#include "generated/debayerParameters.h"
#include "ui_debayerParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class DebayerParametersControlWidget;
}

class DebayerParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit DebayerParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~DebayerParametersControlWidget();

    DebayerParameters* createParameters() const;
    void getParameters(DebayerParameters &param) const;
    void setParameters(const DebayerParameters &input);
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
    Ui_DebayerParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

