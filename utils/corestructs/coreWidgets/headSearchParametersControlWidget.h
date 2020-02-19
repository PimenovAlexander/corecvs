#pragma once
#include <QWidget>
#include "core/xml/generated/headSearchParameters.h"
#include "ui_headSearchParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


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
    void getParameters(HeadSearchParameters &param) const;
    void setParameters(const HeadSearchParameters &input);
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
    Ui_HeadSearchParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

