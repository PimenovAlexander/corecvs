#pragma once
#include <QWidget>
#include "generated/openCVFilterParameters.h"
#include "ui_openCVFilterParametersControlWidget.h"
#include "filterParametersControlWidgetBase.h"


namespace Ui {
    class OpenCVFilterParametersControlWidget;
}

class OpenCVFilterParametersControlWidget : public FilterParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit OpenCVFilterParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~OpenCVFilterParametersControlWidget();

    OpenCVFilterParameters* createParameters() const;
    void setParameters(const OpenCVFilterParameters &input);
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
    Ui_OpenCVFilterParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

