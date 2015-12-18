#pragma once
#include <QtGui/QWidget>
#include "generated/graphPlotParameters.h"
#include "ui_graphPlotParametersControlWidget.h"
#include "parametersControlWidgetBase.h"

namespace Ui {
    class GraphPlotParametersControlWidget;
}

class GraphPlotParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit GraphPlotParametersControlWidget(QWidget *parent = 0, bool autoInit = false, QString rootPath = QString());
    ~GraphPlotParametersControlWidget();

    GraphPlotParameters* createParameters() const;
    void setParameters(const GraphPlotParameters &input);
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
    Ui_GraphPlotParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};
