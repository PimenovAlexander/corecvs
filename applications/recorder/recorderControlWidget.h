
#pragma once
#include <QWidget>
#include <QLineEdit>
#include "generatedParameters/recorder.h"
#include "ui_recorderControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"

namespace Ui {
    class RecorderControlWidget;
}

class RecorderControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit RecorderControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~RecorderControlWidget();

    Recorder* createParameters() const;

    void getParameters(Recorder &params) const;
    void setParameters(const Recorder &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::RecorderControlWidget *ui()
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
    Ui::RecorderControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


