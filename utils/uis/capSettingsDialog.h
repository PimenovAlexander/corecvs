#ifndef CAPSETTINGSDIALOG_H
#define CAPSETTINGSDIALOG_H


#include <QtCore/QSignalMapper>
#include <QtCore/QMap>
#include <QtGui/QWidget>

#include "cameraControlParameters.h"
#include "imageCaptureInterface.h"
#include "ui_capSettingsDialog.h"
#include "parameterSlider.h"
#include "parametersControlWidgetBase.h"


namespace Ui {
    class CapSettingsDialog;
}

class CapSettingsDialog : public QWidget, public SaveableWidget {
    Q_OBJECT
public:
    CapSettingsDialog(QWidget *parent = 0, ImageCaptureInterface *pInterface = 0);
    ~CapSettingsDialog();

    void clearDialog();
    void refreshDialog();

    ParameterEditorWidget *getParameter(int i)
    {
        return sliders[i];
    }

    virtual void loadFromQSettings (const QString &fileName, const QString &_root);
    virtual void saveToQSettings   (const QString &fileName, const QString &_root);


public slots:
    void parameterChanged(int id);
    void resetPressed(int id);
    void resetAllPressed();

    void refreshLimitsPressed();
protected:
    void changeEvent(QEvent *e);
    void showEvent(QShowEvent *e);

private:
    Ui_CapSettingsDialog *mUi;
    ImageCaptureInterface *mCaptureInterface;

    QSignalMapper *signalMapper;
    QSignalMapper *resetSignalMapper;
    QMap<int, ParameterEditorWidget *> sliders;

    CameraParameters mLeftCameraParameters;
    CameraParameters mRightCameraParameters;
};

#endif // CAPSETTINGSDIALOG_H
