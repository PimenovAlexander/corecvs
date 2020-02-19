
#pragma once
#include <QWidget>
#include <QLineEdit>
#include "generatedParameters/copter.h"
#include "ui_copterControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"

namespace Ui {
    class CopterControlWidget;
}

class CopterControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit CopterControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~CopterControlWidget();

    Copter* createParameters() const;

    void getParameters(Copter &params) const;
    void setParameters(const Copter &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

    Ui::CopterControlWidget *ui()
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
    Ui::CopterControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


