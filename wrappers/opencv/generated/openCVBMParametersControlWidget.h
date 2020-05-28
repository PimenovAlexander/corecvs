#pragma once
#include <QWidget>
#include "openCVBMParameters.h"
#include "ui_openCVBMParametersControlWidget.h"
#include "utils/corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class OpenCVBMParametersControlWidget;
}

class OpenCVBMParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit OpenCVBMParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~OpenCVBMParametersControlWidget();

    OpenCVBMParameters* createParameters() const;
    void getParameters(OpenCVBMParameters &params) const;
    void setParameters(const OpenCVBMParameters &input);
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
    Ui_OpenCVBMParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

