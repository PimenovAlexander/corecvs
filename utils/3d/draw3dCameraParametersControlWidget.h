#pragma once
#include <QWidget>
#include "generated/draw3dCameraParameters.h"
#include "ui_draw3dCameraParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class Draw3dCameraParametersControlWidget;
}

class Draw3dCameraParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit Draw3dCameraParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~Draw3dCameraParametersControlWidget();

    Draw3dCameraParameters* createParameters() const;
    void getParameters(Draw3dCameraParameters &params) const;
    void setParameters(const Draw3dCameraParameters &input);
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
    Ui_Draw3dCameraParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
