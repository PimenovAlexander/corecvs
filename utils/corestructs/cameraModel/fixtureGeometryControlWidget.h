#ifndef FIXTUREGEOMETRYCONTROLWIDGET_H
#define FIXTUREGEOMETRYCONTROLWIDGET_H

#include <QWidget>

#include "camerafixture/sceneFeaturePoint.h"
#include "camerafixture/fixtureScenePart.h"
#include "corestructs/parametersControlWidgetBase.h"

#include "uis/graphPlotDialog.h"

namespace Ui {
class FixtureGeometryControlWidget;
}

class FixtureGeometryControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit FixtureGeometryControlWidget(QWidget *parent = 0);
    ~FixtureGeometryControlWidget();

    corecvs::FlatPolygon* createParameters() const;
    void getParameters(corecvs::FlatPolygon &params) const;
    void setParameters(const corecvs::FlatPolygon &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &/*loader*/);
    virtual void saveParamWidget(WidgetSaver  &/*saver*/ );

public slots:
    void paramsChangedInUI();
    void updateHelperUI();


signals:
    void valueChanged();
    void paramsChanged();


private:
    Ui::FixtureGeometryControlWidget *ui;

/* This part is FlatPolygon unrelated. Code above would be decoupled */
public:
    corecvs::FixtureSceneGeometry *mGeometry = NULL;
    void setFixtureGeometry(corecvs::FixtureSceneGeometry *geometry);
    RGBColor getColor();

    GraphPlotDialog mGraphDialog;

public slots:
    void showRelatedGraph();
    void hilightRelatedGraph();
    void regenGraph();

};



#endif // FIXTUREGEOMETRYCONTROLWIDGET_H
