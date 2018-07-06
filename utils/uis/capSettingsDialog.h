#ifndef CAPSETTINGSDIALOG_H
#define CAPSETTINGSDIALOG_H

#include <QtCore/QSignalMapper>
#include <QtCore/QMap>
#include <QWidget>

#include "core/framesources/cameraControlParameters.h"
#include "core/framesources/imageCaptureInterface.h"
#include "ui_capSettingsDialog.h"
#include "parameterSlider.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class CapSettingsDialog;
}

class CapSettingsDialog : public QWidget, public SaveableWidget
{
    Q_OBJECT

public:
    CapSettingsDialog(QWidget *parent = NULL, QString rootPath = "");
   ~CapSettingsDialog();

    void setCaptureInterface(ImageCaptureInterface *pInterface = NULL, bool updateOnFly = true);
    void clearDialog();
    void refreshDialog();

    virtual void loadFromQSettings (const QString &fileName, const QString &_root, bool interfaceGroup = true);
    virtual void saveToQSettings   (const QString &fileName, const QString &_root, bool interfaceGroup = true);

public slots:
    void parameterChanged(int id);
    void resetPressed(int id);
    void newCameraParamValue(int id);
    void resetAllPressed();
    void refreshLimitsPressed();
    void updateAllPressed();

protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);

private:
    //ParameterEditorWidget *getParameter(int i) { return sliders[i]; }

    QString                            mRootPath;
    Ui_CapSettingsDialog              *mUi;
    ImageCaptureInterface             *mCaptureInterface;
    bool                               mUpdateOnFly;
    CameraParameters                   mCameraParameters;

    QSignalMapper                     *signalMapper;
    QSignalMapper                     *resetSignalMapper;
    QMap<int, ParameterEditorWidget *> sliders;
};

#endif // CAPSETTINGSDIALOG_H
