#ifndef POINTERFIELDWIDGET_H
#define POINTERFIELDWIDGET_H

#include "core/reflection/reflection.h"
#include <QWidget>
//#include "cloudViewDialog.h"
#include "advancedImageWidget.h"
#include "cameraModelParametersControlWidget.h"

namespace Ui {
class PointerFieldWidget;
}

class CloudViewDialog;

class PointerFieldWidget : public QWidget
{
    Q_OBJECT

public:
    const corecvs::PointerField *fieldReflection = NULL;


    explicit PointerFieldWidget(const corecvs::PointerField *field, QWidget *parent = 0);
    ~PointerFieldWidget();

    void *rawPointer;

    void  setValue(void *value);
    void *getValue();

    /* Temporary. This should be made pluggable ASAP  */
    AdvancedImageWidget                *image         = NULL;
    CameraModelParametersControlWidget *cameraControl = NULL;
    CloudViewDialog                    *cloud         = NULL;


public slots:
    /* This should be done pluggable and a wrapper style around core structures */
    /* And obiously not be in the same class */
    void loadRGB24Buffer();
    void showRGB24Buffer();

    void loadG12Buffer();
    void showG12Buffer();

    void loadFixtureScene();
    void showFixtureScene();

    void showFixtureCamera();
    void saveFixtureCamera();

    void showMesh3d();
    void saveMesh3d();


private:
    Ui::PointerFieldWidget *ui;
};

#endif // POINTERFIELDWIDGET_H
