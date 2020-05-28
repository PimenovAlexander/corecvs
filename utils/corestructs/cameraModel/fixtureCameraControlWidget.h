#ifndef FIXTURECAMERACONTROLWIDGET_H
#define FIXTURECAMERACONTROLWIDGET_H

#include "camerafixture/fixtureCamera.h"

#include <QWidget>
#include <corestructs/parametersControlWidgetBase.h>

namespace Ui {
class FixtureCameraControlWidget;
}

class FixtureCameraControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit FixtureCameraControlWidget(QWidget *parent = 0);
    ~FixtureCameraControlWidget();

    void getParameters(FixtureCamera &params) const;
    void setParameters(const FixtureCamera &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &/*loader*/);
    virtual void saveParamWidget(WidgetSaver  &/*saver*/ );

signals:
    void valueChanged();
    void paramsChanged();


    void loadRequest(QString filename);
    void saveRequest(QString filename);

private:
    Ui::FixtureCameraControlWidget *ui;
};

#endif // FIXTURECAMERACONTROLWIDGET_H
