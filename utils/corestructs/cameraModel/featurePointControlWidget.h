#ifndef FEATUREPOINTCONTROLWIDGET_H
#define FEATUREPOINTCONTROLWIDGET_H

#include <QWidget>

#include "core/camerafixture/sceneFeaturePoint.h"

namespace Ui {
class FeaturePointControlWidget;
}

class FeaturePointControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FeaturePointControlWidget(QWidget *parent = 0);
    ~FeaturePointControlWidget();

    //SceneFeaturePoint getParameters() const;
    void getParameters(SceneFeaturePoint &params) const;


    void setParameters(const SceneFeaturePoint &input);
    virtual void setParametersVirtual(void *input);

    void setEnabled(bool flag);

public slots:

    void posToReproj();
    void reprojToPos();

signals:
    void paramsChanged();

private:
    Ui::FeaturePointControlWidget *ui;
};

#endif // FEATUREPOINTCONTROLWIDGET_H
