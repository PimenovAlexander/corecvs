#ifndef CAMERACALCULATORWIDGET_H
#define CAMERACALCULATORWIDGET_H

#include <QtGui/QWidget>
#include "ui_cameraCalculatorWidget.h"
#include "colorTimer.h"

struct SensorPreset
{
	const char* name;
	double imagerDiag;
	double imagerW;
	double imagerH;
};

struct LensPreset
{
	const char* name;
	double focal;
	int matrixPreset;
};

class CameraCalculatorWidget : public QWidget
{
    Q_OBJECT

public:
    static SensorPreset sensorPresets[];
    static const int sensorPresetsSize = 34;

    static LensPreset lensPersets[];
    static const int lensPresetsSize = 7;

    CameraCalculatorWidget(QWidget *parent = 0);
    ~CameraCalculatorWidget();

public slots:
	void sensorPresetChanged();
	void resolutionChanged();
	void aspectChanged();
	void aspectAutoChanged();

	/* Different sizes */
	void sensorHorizontalChanged();
	void sensorVerticalChanged();
	void sensorDiagonalChanged();

	/* Lens related */
	void lensPresetChanged();
	void lensFocalChanged();
	void lensNominalAngleChanged();
    void lensRealAngleChanged();

    /*Object related */
    void objectSizeChanged();
    void objectPxSizeChanged();
    void objectDistanceChanged();

    /*Stereo related */
    void stereoBaseLineChanged();
    void stereoAlgorithPrecisionChanged();

private:
    Ui::CameraCalculatorWidgetClass ui;

    static void setValueBlocking(QDoubleSpinBox *box, double value);
    void setNewValue(QDoubleSpinBox *box, double value);
    void updateSensorStats(void);
    void updateLensStats  (void);
    void updateStereoStats(void);
    void startTimer(QDoubleSpinBox *spinBox);

    double getPixelHeight();
    double getPixelWidth();
    double getAlgorithmPrecision();
    std::vector<ColorTimer *> mTimers;

private slots:
    void setColor(QDoubleSpinBox *changedValueBox, int timeoutNum);
    void setDisparity();
};

#endif // CAMERACALCULATORWIDGET_H
