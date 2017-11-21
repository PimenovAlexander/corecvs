#ifndef FIXTUREGLOBALPARAMETERSWIDGET_H
#define FIXTUREGLOBALPARAMETERSWIDGET_H

#include <QWidget>

#include "core/camerafixture/fixtureScene.h"

namespace Ui {
class FixtureGlobalParametersWidget;
}

class FixtureGlobalParametersWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FixtureGlobalParametersWidget(QWidget *parent = 0);
    ~FixtureGlobalParametersWidget();

    void setData(const FixtureScene *scene);

public slots:

    void changePath();
    void replacePath();


private:
    Ui::FixtureGlobalParametersWidget *ui;
};

#endif // FIXTUREGLOBALPARAMETERSWIDGET_H
