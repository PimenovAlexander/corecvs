#pragma once
#include <QWidget>
#include <QtCore/QDebug>

#include "generatedParameters/baseParameters.h"
#include "ui_baseParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"

namespace Ui {
    class BaseParametersControlWidget;
}

class BaseParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit BaseParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~BaseParametersControlWidget();

    BaseParameters* createParameters() const;

    void getParameters(BaseParameters &params) const;
    void setParameters(const BaseParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::BaseParametersControlWidget *ui()
    {
        return mUi;
    }

public slots:
    void changeParameters()
    {
        // emit paramsChanged();
    }

signals:
    void valueChanged();
    void paramsChanged();



private:
    Ui::BaseParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


