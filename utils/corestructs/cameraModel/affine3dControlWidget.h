#ifndef CAMERALOCATIONDATACONTROLWIDGET_H
#define CAMERALOCATIONDATACONTROLWIDGET_H

#include <QDialog>

#include "core/cameracalibration/calibrationLocation.h"
#include "parametersControlWidgetBase.h"

namespace Ui {
class Affine3dControlWidget;
}

class Affine3dControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    explicit Affine3dControlWidget(QWidget *parent = 0, bool presenationWorldStyle = false);
    ~Affine3dControlWidget();

    Affine3DQ* createParameters() const;
    void getParameters(Affine3DQ &params) const;
    void setParameters(const Affine3DQ &input);
    virtual void setParametersVirtual(void *input);

public slots:
    void copyText();
    void setPresentationStyle(bool worldCoord);
signals:
    void valueChanged();
    void paramsChanged();

private:
    bool mWorld;
    Ui::Affine3dControlWidget *ui;
};

#endif // CAMERALOCATIONDATACONTROLWIDGET_H
