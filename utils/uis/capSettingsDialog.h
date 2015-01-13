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
    CapSettingsDialog(QWidget *parent = NULL, QString rootPath = "");
    ~CapSettingsDialog();


    void setCaptureInterface(ImageCaptureInterface *pInterface = NULL);
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
    QString mRootPath;

    Ui_CapSettingsDialog *mUi;
    ImageCaptureInterface *mCaptureInterface;

    QSignalMapper *signalMapper;
    QSignalMapper *resetSignalMapper;
    QMap<int, ParameterEditorWidget *> sliders;

    CameraParameters mLeftCameraParameters;
    CameraParameters mRightCameraParameters;
};

#endif // CAPSETTINGSDIALOG_H
