#pragma once
#include <QWidget>
#include "core/xml/generated/euclidianMoveParameters.h"
#include "ui_euclidianMoveParametersControlWidget.h"
#include "corestructs/parametersControlWidgetBase.h"


namespace Ui {
    class EuclidianMoveParametersControlWidget;
}

class EuclidianMoveParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit EuclidianMoveParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~EuclidianMoveParametersControlWidget();

    EuclidianMoveParameters* createParameters() const;
    void getParameters(EuclidianMoveParameters &param) const;
    void setParameters(const EuclidianMoveParameters &input);
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
    Ui_EuclidianMoveParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};

