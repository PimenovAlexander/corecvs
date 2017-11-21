#pragma once
#include <QWidget>
#include "core/xml/generated/makePreciseParameters.h"
#include "ui_makePreciseParametersControlWidget.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class MakePreciseParametersControlWidget;
}

class MakePreciseParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit MakePreciseParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~MakePreciseParametersControlWidget();

    MakePreciseParameters* createParameters() const;
    void getParameters(MakePreciseParameters &param) const;
    void setParameters(const MakePreciseParameters &input);
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
    Ui_MakePreciseParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

