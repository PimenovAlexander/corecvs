#pragma once
#include <QtGui/QWidget>

#include "generated/headSearchParameters.h"
#include "ui_headSearchParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class HeadSearchParametersControlWidget;
}

class HeadSearchParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit HeadSearchParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~HeadSearchParametersControlWidget();

    HeadSearchParameters* createParameters() const;
    void setParameters(const HeadSearchParameters &input);

    void setParametersVirtual(void *input);

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
    Ui_HeadSearchParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

