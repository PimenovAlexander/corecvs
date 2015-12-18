#pragma once
#include <QWidget>
#include "generated/draw3dViMouseParameters.h"
#include "ui_draw3dViMouseParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class Draw3dViMouseParametersControlWidget;
}

class Draw3dViMouseParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit Draw3dViMouseParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~Draw3dViMouseParametersControlWidget();

    Draw3dViMouseParameters* createParameters() const;
    void getParameters(Draw3dViMouseParameters &params) const;
    void setParameters(const Draw3dViMouseParameters &input);
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
    Ui_Draw3dViMouseParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
