#pragma once
#include <QWidget>
#include "generated/draw3dParameters.h"
#include "ui_draw3dParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class Draw3dParametersControlWidget;
}

class Draw3dParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit Draw3dParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~Draw3dParametersControlWidget();

    Draw3dParameters* createParameters() const;
    void getParameters(Draw3dParameters &params) const;
    void setParameters(const Draw3dParameters &input);
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
    Ui_Draw3dParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
