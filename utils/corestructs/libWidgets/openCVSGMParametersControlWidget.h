#pragma once
#include <QWidget>
#include "openCVSGMParameters.h"
#include "ui_openCVSGMParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class OpenCVSGMParametersControlWidget;
}

class OpenCVSGMParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit OpenCVSGMParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~OpenCVSGMParametersControlWidget();

    OpenCVSGMParameters* createParameters() const;
    void setParameters(const OpenCVSGMParameters &input);
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
    Ui_OpenCVSGMParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

