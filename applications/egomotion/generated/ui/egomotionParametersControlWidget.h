#pragma once
#include <QWidget>
#include "generated/egomotionParameters.h"
#include "ui_egomotionParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class EgomotionParametersControlWidget;
}

class EgomotionParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit EgomotionParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~EgomotionParametersControlWidget();

    EgomotionParameters* createParameters() const;
    void getParameters(EgomotionParameters &param) const;
    void setParameters(const EgomotionParameters &input);
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
    Ui_EgomotionParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

