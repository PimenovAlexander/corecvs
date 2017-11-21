#ifndef rectifyWidget_H
#define rectifyWidget_H

#include <QDialog>
#include "ui_rectifyParametersControlWidget.h"
#include "core/math/eulerAngles.h"
#include "core/math/vector/vector3d.h"
#include "core/math/mathUtils.h"
#include "rectifyParameters.h"
#include "parametersControlWidgetBase.h"
#include "qtHelper.h"

using namespace corecvs;

class RectifyParametersControlWidget : public ParametersControlWidgetBase
{
    Q_OBJECT

public:
    RectifyParametersControlWidget(QWidget *parent, bool _autoInit, QString _rootPath);
    ~RectifyParametersControlWidget();

    QPushButton *getRectifyButton()
    {
        return mUi->rectifyButton;
    }

    QPushButton *getResetButton()
    {
        return mUi->resetButton;
    }

    QPushButton *getPointsMatchingButton()
    {
        return mUi->bPointsMatching;
    }

    void setManualTab(CameraAnglesLegacy &angle, Vector3dd &dir)
    {
        double pitchDelta = mUi->manualPitchSpinBox->value() - angle.pitch();
        double yawDelta   = mUi->manualYawSpinBox->value()   - angle.yaw();
        double rollDelta  = mUi->manualRollSpinBox->value()  - angle.roll();

        Vector3dd old(mUi->manualXSpinBox->value(), mUi->manualYSpinBox->value(), mUi->manualZSpinBox->value());
        double shiftDelta = (old - dir).l2Metric() * mUi->baselineLengthSpinBox->value();

        QString deltaText = QString("pitch=%1deg\nyaw=%1deg\nroll=%1deg\ndelta=%1unit")
                .arg(radToDeg(pitchDelta))
                .arg(radToDeg(yawDelta))
                .arg(radToDeg(rollDelta))
                .arg(shiftDelta);

        mUi->deltaLabel->setText(deltaText);

        mUi->manualPitchSpinBox->setValue(angle.pitch());
        mUi->manualYawSpinBox  ->setValue(angle.yaw());
        mUi->manualRollSpinBox ->setValue(angle.roll());

        mUi->manualXSpinBox->setValue(dir.x());
        mUi->manualYSpinBox->setValue(dir.y());
        mUi->manualZSpinBox->setValue(dir.z());
    }

    RectifyParameters* createParameters() const;
    void getParameters(RectifyParameters &params) const;
    void setParameters(const RectifyParameters &input);
    virtual void setParametersVirtual(void *input);


    virtual void loadParamWidget(WidgetLoader &loader);
    virtual void saveParamWidget(WidgetSaver  &saver);

signals:
    void paramsChanged();

protected:
    Ui_RectifyParametersControlWidget *mUi;
    bool autoInit;
    QString rootPath;
};


class AutomatedRectifyControlWidget : public RectifyParametersControlWidget
{
    Q_OBJECT

    int framesH;
    int framesW;

public:
    explicit AutomatedRectifyControlWidget(QWidget *parent, bool _autoInit, QString _rootPath) :
        RectifyParametersControlWidget(parent, _autoInit, _rootPath)
      , framesH(0)
      , framesW(0)
    {
        connect(mUi->iterativeIterationsSpinBox  , SIGNAL(valueChanged(int)),    this, SLOT(updateFinalSigma()));
        connect(mUi->iterativeInitialSigmaSpinBox, SIGNAL(valueChanged(double)), this, SLOT(updateFinalSigma()));
        connect(mUi->iterativeFactorSigmaSpinBox , SIGNAL(valueChanged(double)), this, SLOT(updateFinalSigma()));

        connect(mUi->cameraPresetComboBox , SIGNAL(activated(int)), this, SLOT(cameraPresetChanged(int)));

        connect(mUi->priorFocalSpinBox      , SIGNAL(valueChanged(double)), this, SLOT(updateFocal2()));
        connect(mUi->focalProportionSpinBox , SIGNAL(valueChanged(double)), this, SLOT(updateFocal2()));


        connect(mUi->fovToFocalButton, SIGNAL(released()), this, SLOT(updateFocalFromFOV()));
        updateFinalSigma();
        setFramesSize(0,0);
    }

public slots:

    void updateFinalSigma (void)
    {
        int    iterations   = mUi->iterativeIterationsSpinBox->value();
        double initialSigma = mUi->iterativeInitialSigmaSpinBox->value();
        double sigmaFactor  = mUi->iterativeFactorSigmaSpinBox->value();

        double finalSigma =  initialSigma * pow(sigmaFactor, iterations);
        mUi->finalSigmaLabel->setText( QString("Final sigma %1px" ).arg(finalSigma));
    }

    void cameraPresetChanged(int preset)
    {
        switch (preset) {
            // C920
            case 0:  mUi->priorFocalSpinBox->setValue(600); break;
            // PS Eye 640x480 wide
            case 1:  mUi->priorFocalSpinBox->setValue(555.0 * 2650.0 / 2505.0); break;
            // PS Eye 640x480 tele
            case 2:  mUi->priorFocalSpinBox->setValue(640.0 / 83.0 * 100.0); break;
            case 3:  mUi->priorFocalSpinBox->setValue(586.0 * 134.0 / 152.0); break;
            default: mUi->priorFocalSpinBox->setValue(603); break;
        }
    }

    void updateFocal2 (void)
    {
        if (mUi->lockedFocalButton->isChecked()) {
            double focal2 = mUi->priorFocalSpinBox->value();
            focal2 *= mUi->focalProportionSpinBox->value() / 100.0;
            setValueBlocking(mUi->priorFocal2SpinBox, focal2);
        }
    }

    void updateFocalFromFOV (void)
    {
        double fov = mUi->fovAngleSpinBox->value();
        double focal = (framesW / 2.0) / tan(degToRad(fov) / 2.0);
        mUi->priorFocalSpinBox->setValue(focal);
    }

    void setFramesSize(int h, int w)
    {
        framesH = h;
        framesW = w;
        mUi->sizeLabel->setText(QString("Size: %1x%2").arg(framesW).arg(framesH));
    }



};


#endif // rectifyWidget_H
