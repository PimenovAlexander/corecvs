#include "cameraCalculatorWidget.h"
#include "core/math/mathUtils.h"

#include <math.h>

#include <QtCore/QDebug>

using corecvs::radToDeg;


SensorPreset CameraCalculatorWidget::sensorPresets[] =
{
        {"1/10\""                     , 1.60 ,   1.28 ,    0.96   },
        {"1/8\""                      , 2.00 ,   1.60 ,    1.20   },
        {"1/6\""                      , 3.00 ,   2.40 ,    1.80   },
        {"1/4\""                      , 4.00 ,   3.20 ,    2.40   },
        {"1/3.6\""                    , 5.00 ,   4.00 ,    3.00   },
        {"1/3.2\""                    , 5.68 ,   4.54 ,    3.42   },
        {"1/3\""                      , 6.00 ,   4.80 ,    3.60   },
        {"1/2.7\""                    , 6.72 ,   5.37 ,    4.04   },
        {"1/2.5\""                    , 7.18 ,   5.76 ,    4.29   },
        {"Pentax Q (1/2.3\")"         , 7.66 ,   6.17 ,    4.55   },
        {"1/2\""                      , 8.00 ,   6.40 ,    4.80   },
        {"1/1.8\""                    , 8.93 ,   7.18 ,    5.32   },
        {"1/1.7\""                    , 9.50 ,   7.60 ,    5.70   },
        {"1/1.6\""                    , 10.07,   8.08 ,    6.01   },
        {"2/3\""                      , 11.00,   8.80 ,    6.60   },
        {"1/1.2\" (Nokia 808 PureView)" , 13.33, 10.67,    8.00   },
        {"Super 16mm"                 , 14.54,   12.52,    7.41   },
        {"Nikon CX and Sony RX100"    , 15.86,   13.20,    8.80   },
        {"1\""                        , 16.00,   12.80,    9.60   },
        {"m4/3 · 4/3\" (Four Thirds)" , 21.60,   17.30,    13     },
        {"1.5"                        , 23.36,   18.70,    14.00  },
        {"Sigma Foveon X3, original"  , 24.90,   20.70,    13.80  },
        {"Canon APS-C"                , 26.70,   22.20,    14.80  },
        {"General APS-C"              ,  28.2,   23.60,    15.60  }, // (Nikon DX, Pentax K, Samsung NX, Sony α & NEX, current Sigma Foveon X3)
        {"Canon APS-H"                , 33.50,   27.90,    18.60  },
        {"35mm Full-frame"            , 43.20,   36   ,    23.9   }, // , (Nikon FX, Sony α, Canon)
        {"Leica S"                    , 54   ,   45   ,    30     },
        {"Pentax 645D"                , 55   ,   44   ,    33     },
        {"Kodak KAF 39000 CCD[26]"    , 61.30,   49   ,    36.80  },
        {"Leaf AFi 10"                , 66.57,   56   ,    36     },
        {"Phase One P 65+, IQ160-180" , 67.40,   53.90,    40.40  },
        {"UEye CP5420"                ,  8.69,    6.784,    5.427 },
        {"Guess C600 640x480"         ,  8.00,    6.40,     4.80  },
        {"Guess C920 1920x1080"       , 22.03,   19.20,    10.80  }

};

STATIC_ASSERT(CORE_COUNT_OF(CameraCalculatorWidget::sensorPresets) == CameraCalculatorWidget::sensorPresetsSize, wrong_number_of_sensor_entries)


LensPreset CameraCalculatorWidget::lensPersets[] =
{
		{"Kowa LM 6NCM", 6.0, 10},
		{"NoName 6mm 1/2\" F1.4 Manual Iris", 6.0, 10},
		{"NoName black 1/2\" 25mm", 25.0 , 10},
		{"PRO-18M1050MP3IR (wide)", 10.0 , 11},
		{"PRO-18M1050MP3IR (tele)", 50.0 , 11},
		{"Guess C600"             ,  5.40, 32},
		{"Guess 1920x1080"        , 14.76, 33},
};

STATIC_ASSERT(CORE_COUNT_OF(CameraCalculatorWidget::lensPersets) == CameraCalculatorWidget::lensPresetsSize, wrong_number_of_sensor_entries)


CameraCalculatorWidget::CameraCalculatorWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    for (int i = 0; i < sensorPresetsSize; i++)
	{
		ui.sensorPresetComboBox->addItem(QString(sensorPresets[i].name));
		ui.lensNominalSensorComboBox->addItem(QString(sensorPresets[i].name));
	}

	for (int i = 0; i < lensPresetsSize; i++)
	{
		ui.lensPresetComboBox->addItem(QString(lensPersets[i].name));
	}

	/* Sensors */
	connect(ui.sensorPresetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sensorPresetChanged()));

	connect(ui.xResolutionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(resolutionChanged()));
	connect(ui.yResolutionSpinBox, SIGNAL(valueChanged(int)), this, SLOT(resolutionChanged()));

	connect(ui.aspectSpinBox, SIGNAL(valueChanged(double)), this, SLOT(aspectChanged()));


	connect(ui.horizontalSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(sensorHorizontalChanged()));
	connect(ui.verticalSizeSpinBox  , SIGNAL(valueChanged(double)), this, SLOT(sensorVerticalChanged()));
	connect(ui.diagonalSizeSpinBox  , SIGNAL(valueChanged(double)), this, SLOT(sensorDiagonalChanged()));

	/* Lens */
	connect(ui.lensPresetComboBox    , SIGNAL(currentIndexChanged(int)), this, SLOT(lensPresetChanged()));
	connect(ui.lensFocalLengthSpinBox, SIGNAL(valueChanged(double)), this, SLOT(lensFocalChanged()));
	connect(ui.lensAOVSpinBox        , SIGNAL(valueChanged(double)), this, SLOT(lensNominalAngleChanged()));
    connect(ui.sensorAOVSpinBox      , SIGNAL(valueChanged(double)), this, SLOT(lensRealAngleChanged()));

    /* Object */
    connect(ui.objectSizeSpinBox     , SIGNAL(valueChanged(double)), this, SLOT(objectSizeChanged()));
    connect(ui.objectPixelSizeSpinBox, SIGNAL(valueChanged(double)), this, SLOT(objectPxSizeChanged()));
    connect(ui.objectDistanceSpinBox , SIGNAL(valueChanged(double)), this, SLOT(objectDistanceChanged()));

    /* Stereo */
    connect(ui.objectSizeSpinBox     , SIGNAL(valueChanged(double))    , this, SLOT(stereoBaseLineChanged()));
    connect(ui.lensPresetComboBox    , SIGNAL(currentIndexChanged(int)), this, SLOT(stereoAlgorithPrecisionChanged()));
    connect(ui.baseLineSpinBox       , SIGNAL(valueChanged(double))   , this, SLOT(setDisparity()));
}

void CameraCalculatorWidget::setValueBlocking(QDoubleSpinBox *box, double value)
{
	bool wasBlocked = box->blockSignals(true);
	box->setValue(value);
    box->blockSignals(wasBlocked);
}

void CameraCalculatorWidget::setNewValue(QDoubleSpinBox *box, double value)
{
    setValueBlocking(box, value);
    startTimer(box);
}

/**
 *  Block corresponding to sensor
 *
 **/
void CameraCalculatorWidget::sensorPresetChanged()
{
	int id = ui.sensorPresetComboBox->currentIndex();
	SensorPreset *preset = &(sensorPresets[id]);

    setNewValue(ui.horizontalSizeSpinBox, preset->imagerW);
    setNewValue(ui.verticalSizeSpinBox  , preset->imagerH);
    setNewValue(ui.diagonalSizeSpinBox  , preset->imagerDiag);
    setNewValue(ui.aspectSpinBox        , (double)preset->imagerH / preset->imagerW);
    updateSensorStats();
}

void CameraCalculatorWidget::resolutionChanged()
{
    if (ui.aspectAutoCheckBox->isChecked())
    {
        ui.aspectSpinBox->setValue((double)ui.xResolutionSpinBox->value() / ui.yResolutionSpinBox->value());
        startTimer(ui.aspectSpinBox);
    }
    setDisparity();
}


void CameraCalculatorWidget::sensorHorizontalChanged()
{
	double horSize = ui.horizontalSizeSpinBox->value();
	double vertSize = horSize / ui.aspectSpinBox->value();
    setNewValue(ui.verticalSizeSpinBox,vertSize);
	double diagSize = sqrt(horSize * horSize + vertSize * vertSize);
    setNewValue(ui.diagonalSizeSpinBox,diagSize);
	updateSensorStats();
}

void CameraCalculatorWidget::sensorVerticalChanged()
{
	double vertSize = ui.verticalSizeSpinBox->value();
	double horSize = vertSize * ui.aspectSpinBox->value();
    setNewValue(ui.horizontalSizeSpinBox,horSize);
	double diagSize = sqrt(horSize * horSize + vertSize * vertSize);
    setNewValue(ui.diagonalSizeSpinBox,diagSize);
    updateSensorStats();
}

void CameraCalculatorWidget::sensorDiagonalChanged()
{
    double const diagonal = ui.diagonalSizeSpinBox->value();
    double const ctg = ui.aspectSpinBox->value();
    double const sin = 1 / sqrt(1 + ctg * ctg);
    double const cos = ctg / sqrt(1 + ctg * ctg);
    setNewValue(ui.horizontalSizeSpinBox, diagonal * cos);
    setNewValue(ui.verticalSizeSpinBox, diagonal * sin);
}


void CameraCalculatorWidget::aspectChanged()
{
    sensorDiagonalChanged();
}

void CameraCalculatorWidget::aspectAutoChanged()
{
	ui.aspectSpinBox->setEnabled(ui.aspectAutoCheckBox->isChecked());
}

/**
 *  Block corresponding to lens
 *
 **/

void CameraCalculatorWidget::lensPresetChanged()
{
	int id = ui.lensPresetComboBox->currentIndex();
	LensPreset *preset = &(lensPersets[id]);
	ui.lensFocalLengthSpinBox->setValue(preset->focal);
    startTimer(ui.lensFocalLengthSpinBox);
	ui.lensNominalSensorComboBox->setCurrentIndex(preset->matrixPreset);
}

void CameraCalculatorWidget::lensFocalChanged()
{
	double focal = ui.lensFocalLengthSpinBox->value();
	int sensId = ui.lensNominalSensorComboBox->currentIndex();
	SensorPreset *preset = &(sensorPresets[sensId]);

	double sensorHor = preset->imagerW;
	double angle = 2.0 * atan2(sensorHor / 2.0, focal);
    setNewValue(ui.lensAOVSpinBox, corecvs::radToDeg(angle));

    sensorHor = ui.horizontalSizeSpinBox->value();
    angle = 2.0 * atan2(sensorHor / 2.0, focal);
    setNewValue(ui.sensorAOVSpinBox, corecvs::radToDeg(angle));

}

void CameraCalculatorWidget::lensNominalAngleChanged()
{
	int sensId = ui.lensNominalSensorComboBox->currentIndex();
	SensorPreset *preset = &(sensorPresets[sensId]);
	double sensorHor = preset->imagerW;

	double tana2 = tan(corecvs::degToRad((double)(ui.lensAOVSpinBox->value()) / 2.0));
	double focal = (sensorHor / 2.0) / tana2;
    setNewValue(ui.lensFocalLengthSpinBox, focal);
}

void CameraCalculatorWidget::lensRealAngleChanged()
{
    double sensorHor = ui.horizontalSizeSpinBox->value();
    double tana2 = tan(corecvs::degToRad((double)(ui.lensAOVSpinBox->value()) / 2.0));
    double focal = (sensorHor / 2.0) / tana2;
    setNewValue(ui.lensFocalLengthSpinBox, focal);
    setDisparity();
}

/*Object related */
// Merge latter together
void CameraCalculatorWidget::objectDistanceChanged()
{
    double distance   = ui.objectDistanceSpinBox->value();
    double objectSize = ui.objectSizeSpinBox->value();
    double focal      = ui.lensFocalLengthSpinBox->value();

    double sizeOnSensor = (objectSize / distance) * focal;

    double sizeInPx = sizeOnSensor / getPixelWidth();

    setNewValue(ui.objectPixelSizeSpinBox, sizeInPx);
    updateStereoStats();
}

void CameraCalculatorWidget::objectSizeChanged()
{
    double distance   = ui.objectDistanceSpinBox->value();
    double objectSize = ui.objectSizeSpinBox->value();
    double focal      = ui.lensFocalLengthSpinBox->value();

    double sizeOnSensor = (objectSize / distance) * focal;

    double sizeInPx = sizeOnSensor / getPixelWidth();

    setNewValue(ui.objectPixelSizeSpinBox, sizeInPx);
}

void CameraCalculatorWidget::objectPxSizeChanged()
{
    double distance     = ui.objectDistanceSpinBox->value();
    double objectPxSize = ui.objectSizeSpinBox->value();
    double focal        = ui.lensFocalLengthSpinBox->value();

    double sizeOnSensor = objectPxSize * getPixelWidth();
    double objectSize = sizeOnSensor / focal * distance;

    setNewValue(ui.objectSizeSpinBox, objectSize);
}


/*Stereo related */
void CameraCalculatorWidget::stereoBaseLineChanged()
{
    qDebug() << "CameraCalculatorWidget::stereoBaseLineChanged called";
    updateStereoStats();
}

void CameraCalculatorWidget::stereoAlgorithPrecisionChanged()
{
    qDebug() << "CameraCalculatorWidget::stereoAlgorithPrecisionChanged called";
    updateStereoStats();
}


void CameraCalculatorWidget::updateSensorStats(void)
{
    QString sensorStats;
    double sensorAreaSize = ui.verticalSizeSpinBox->value() * ui.horizontalSizeSpinBox->value();
    sensorStats += QString("Sensor Area: %1 mm^2\n").arg(sensorAreaSize);


    sensorStats += QString("Pixel Width: %1 mm\n").arg(getPixelWidth());
    sensorStats += QString("Pixel Height: %1 mm\n").arg(getPixelHeight());

    ui.sensorInfoLabel->setText(sensorStats);
}

void CameraCalculatorWidget::updateStereoStats(void)
{
    double distance = ui.objectDistanceSpinBox->value();
    double baseline = ui.baseLineSpinBox->value();

    double precision = getAlgorithmPrecision();
    double pixelSize = getPixelWidth();

    double zPrec = (baseline / ((baseline / distance) - precision * pixelSize)) - distance;

    QString stereoStats;
    stereoStats += QString("Z precision: %1 mm\n").arg(zPrec);
    ui.stereoInfoLabel->setText(stereoStats);
}

CameraCalculatorWidget::~CameraCalculatorWidget()
{
    foreach (QTimer *timer, mTimers)
    {
        delete timer;
    }
}


double CameraCalculatorWidget::getPixelHeight()
{
    return ui.verticalSizeSpinBox->value() / ui.yResolutionSpinBox->value();
}

double CameraCalculatorWidget::getPixelWidth()
{
    return ui.horizontalSizeSpinBox->value() / ui.xResolutionSpinBox->value();
}

double CameraCalculatorWidget::getAlgorithmPrecision()
{
   switch (ui.stereoAlgorithmPrecBox->currentIndex())
   {
       default:
       case 0: return 1.0;
       case 1: return 1.0 / 2.0;
       case 2: return 1.0 / 4.0;
       case 3: return 1.0 / 8.0;
   }
}

void CameraCalculatorWidget::setColor(QDoubleSpinBox *changedValueBox, int timeoutNum)
{
    if (timeoutNum > 6)
    {
        return;
    }
    Qt::GlobalColor color = (timeoutNum % 2 == 1) ? Qt::red : Qt::black;
    QPalette palette = changedValueBox->palette();
    palette.setColor(QPalette::Text, color);
    changedValueBox->setPalette(palette);
}

void CameraCalculatorWidget::startTimer(QDoubleSpinBox *spinBox)
{
    ColorTimer *timer = new ColorTimer(spinBox);
    connect(timer, SIGNAL(colorTimeout(QDoubleSpinBox*, int)), this, SLOT(setColor(QDoubleSpinBox*, int)));
    timer->start(150);
    mTimers.push_back(timer);
}

void CameraCalculatorWidget::setDisparity()
{
    double const pixelSize = atan(ui.sensorAOVSpinBox->value()) * 2.0 / ui.xResolutionSpinBox->value();
    double const baseline = ui.baseLineSpinBox->value();
    setNewValue(ui.disparitySpinBox, (sqrt(pixelSize * pixelSize + 4 * pixelSize * baseline) - pixelSize) * 0.5);
}
