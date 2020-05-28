#pragma once
#include <QWidget>
#include "core/xml/generated/distortionApplicationParameters.h"
#include "ui_distortionApplicationParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class DistortionApplicationParametersControlWidget;
}

class DistortionApplicationParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit DistortionApplicationParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~DistortionApplicationParametersControlWidget();

    DistortionApplicationParameters* createParameters() const;
    void getParameters(DistortionApplicationParameters &param) const;
    void setParameters(const DistortionApplicationParameters &input);
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
    Ui_DistortionApplicationParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

