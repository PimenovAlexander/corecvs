#include <QMessageBox>
#include <QFileDialog>
#include "core/buffers/rgb24/abstractPainter.h"
#include "qtFileLoader.h"

#include "lensDistortionModelParametersControlWidget.h"
#include "ui_lensDistortionModelParametersControlWidget.h"
#include "core/buffers/displacementBuffer.h"
#include "g12Image.h"

LensDistortionModelParametersControlWidget::LensDistortionModelParametersControlWidget(QWidget *parent) :
    ParametersControlWidgetBase(parent),
    ui(new Ui::LensDistortionModelParametersContolWidget),
    rootPath("intrinsic"),
    mExample(NULL),
    mInput(NULL),
    mCorrected(NULL),
    mInverse(NULL),
    mBackproject(NULL),
    mDiff(NULL),
    mIsolines(NULL)
{
    ui->setupUi(this);

    QObject::connect(ui->centerXSpinBox,     SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->centerYSpinBox,     SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->tangential1SpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->tangential2SpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->scaleSpinBox,       SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->aspectSpinBox,      SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->normalizerSpinBox,  SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->shiftXSpinBox,      SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(ui->shiftYSpinBox,      SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->forwardMapCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->koefTableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SIGNAL(paramsChanged()));



    QObject::connect(ui->freeParamSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));

    QObject::connect(ui->loadPushButton, SIGNAL(released()), this, SLOT(loadParams()));
    QObject::connect(ui->savePushButton, SIGNAL(released()), this, SLOT(saveParams()));

    QObject::connect(this, SIGNAL(paramsChanged()), this, SLOT(updateAdditionalDataNeeded()));
    QObject::connect(ui->refreshButton, SIGNAL(released()), this, SLOT(updateAdditionalData()));
    QObject::connect(ui->autoRefeshCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateAdditionalDataNeeded()));


    QObject::connect(ui->advancedButton, SIGNAL(toggled(bool)), this, SLOT(toggleAdvanced(bool)));

}

LensDistortionModelParametersControlWidget::~LensDistortionModelParametersControlWidget()
{
    delete ui;
}

void LensDistortionModelParametersControlWidget::loadParamWidget(WidgetLoader &loader)
{
    LensDistortionModelParameters *params = createParameters();
    loader.loadParameters(*params, rootPath);
    setParameters(*params);
    delete params;
}

void LensDistortionModelParametersControlWidget::saveParamWidget(WidgetSaver  &saver)
{
    LensDistortionModelParameters *params = createParameters();
    saver.saveParameters(*params, rootPath);
    delete params;
}

void LensDistortionModelParametersControlWidget::getParameters(LensDistortionModelParameters &params) const
{
    params.setPrincipalX(ui->centerXSpinBox->value());
    params.setPrincipalY(ui->centerYSpinBox->value());
    params.setShiftX(ui->shiftXSpinBox->value());
    params.setShiftY(ui->shiftYSpinBox->value());

    params.setTangentialX(ui->tangential1SpinBox->value());
    params.setTangentialY(ui->tangential2SpinBox->value());

    params.setAspect(ui->aspectSpinBox->value());
    params.setScale (ui->scaleSpinBox->value());
    params.setNormalizingFocal(ui->normalizerSpinBox->value());

    params.setMapForward(ui->forwardMapCheckBox->isChecked());

    params.mKoeff.clear();
    for (int i = 0; i < ui->koefTableWidget->rowCount(); i++)
    {
        QDoubleSpinBox *box = static_cast<QDoubleSpinBox *>(ui->koefTableWidget->cellWidget(i,COLUMN_EDIT));
        double value = 0.0;
        if (box != NULL) {
            QVariant str = box->value();
            value = str.toDouble();
        }
        params.mKoeff.push_back(value);
    }
}

LensDistortionModelParameters *LensDistortionModelParametersControlWidget::createParameters() const
{

    /**
     * We should think of returning parameters by value or saving them in a preallocated place
     **/

    LensDistortionModelParameters *result = new LensDistortionModelParameters( );
    getParameters(*result);
    return result;
}

LensDistortionModelParameters LensDistortionModelParametersControlWidget::getParameters() const
{
    LensDistortionModelParameters toReturn;
    getParameters(toReturn);
    return toReturn;
}

void LensDistortionModelParametersControlWidget::setParameters(const LensDistortionModelParameters &input)
{
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);

    ui->centerXSpinBox->setValue(input.principalX());
    ui->centerYSpinBox->setValue(input.principalY());
    ui->shiftXSpinBox->setValue(input.shiftX());
    ui->shiftYSpinBox->setValue(input.shiftY());

    ui->tangential1SpinBox->setValue(input.tangentialX());
    ui->tangential2SpinBox->setValue(input.tangentialY());

    ui->scaleSpinBox->setValue(input.scale());
    ui->aspectSpinBox->setValue(input.aspect());
    ui->normalizerSpinBox->setValue(input.normalizingFocal());

    ui->forwardMapCheckBox->setChecked(input.mapForward());

    ui->koefTableWidget->setRowCount(0);
    for (unsigned i = 0; i < input.mKoeff.size(); i++)
    {
        addPower();
    }

    for (unsigned i = 0; i < input.mKoeff.size(); i++)
    {
        QWidget* widget = ui->koefTableWidget->cellWidget(i,COLUMN_EDIT);
        if (widget == NULL)
        {
            qDebug() << "LensDistortionModelParametersControlWidget::setParameters(): we have internal problem";
            break;
        }
        QDoubleSpinBox *box = static_cast<QDoubleSpinBox *>(widget);
        box->setValue(input.mKoeff[i]);
    }

    blockSignals(wasBlocked);
    emit paramsChanged();
}

void LensDistortionModelParametersControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    LensDistortionModelParameters *inputCasted = static_cast<LensDistortionModelParameters *>(input);
    setParameters(*inputCasted);
}


/* UI related stuff */

void LensDistortionModelParametersControlWidget::addPower()
{
    int newRow = ui->koefTableWidget->rowCount() + 1;
    ui->koefTableWidget->setRowCount(newRow);
    ui->koefTableWidget->setColumnCount(2);
    ui->koefTableWidget->setItem(newRow - 1, COLUMN_CAPTION, new QTableWidgetItem(QString("x^%1").arg(newRow)));

    QDoubleSpinBox *box = new QDoubleSpinBox();
    box->setDecimals(9);
    box->setSingleStep(0.01);
    box->setMaximum( 999999999);
    box->setMinimum(-999999999);
    ui->koefTableWidget->setCellWidget(newRow - 1, COLUMN_EDIT, box);

    connect(box, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    ui->koefTableWidget->setColumnWidth(0, 60);
    ui->koefTableWidget->setColumnWidth(1, 250);

}

void LensDistortionModelParametersControlWidget::delPower()
{
    ui->koefTableWidget->setRowCount(ui->koefTableWidget->rowCount() - 1);
}

void LensDistortionModelParametersControlWidget::resetCx()
{
    if (mExample != NULL)
    {
        ui->centerXSpinBox->setValue(mExample->width() / 2);
    } else {
        ui->centerXSpinBox->setValue(100);
    }
}


void LensDistortionModelParametersControlWidget::resetCy()
{
    if (mExample != NULL)
    {
        ui->centerYSpinBox->setValue(mExample->height() / 2);
    } else {
        ui->centerYSpinBox->setValue(100);
    }
}

void LensDistortionModelParametersControlWidget::resetSx()
{
    ui->shiftXSpinBox->setValue(0);
}


void LensDistortionModelParametersControlWidget::resetSy()
{
    ui->shiftYSpinBox->setValue(0);
}

void LensDistortionModelParametersControlWidget::resetP1()
{
    ui->tangential1SpinBox->setValue(0.0);
}

void LensDistortionModelParametersControlWidget::resetP2()
{
    ui->tangential2SpinBox->setValue(0.0);
}

void LensDistortionModelParametersControlWidget::resetAspect()
{
    ui->aspectSpinBox->setValue(1.0);
}

void LensDistortionModelParametersControlWidget::resetScale()
{
    ui->scaleSpinBox->setValue(1.0);
}

void LensDistortionModelParametersControlWidget::resetNormalize()
{
    if (ui->centerXSpinBox->value() != 0 ||  ui->centerYSpinBox->value() != 0)
    {
        Vector2dd center(ui->centerXSpinBox->value(), ui->centerYSpinBox->value());
        ui->normalizerSpinBox->setValue(center.l2Metric());
    } else {
        ui->centerYSpinBox->setValue(140);
    }

}

void LensDistortionModelParametersControlWidget::invertCorrection()
{
    int result = QMessageBox::information(this, "Warning", "This will invert the radial distortion. Could take some time",
                             QMessageBox::Ok, QMessageBox::Cancel);

    if (result != QMessageBox::Ok) {
        return;
    }

    LensDistortionModelParameters lensParams = LensDistortionModelParametersControlWidget::getParameters();
    RadialCorrection radialCorrection(lensParams);

    setCursor(Qt::BusyCursor);
    RadialCorrection inverted = radialCorrection.invertCorrection(radialCorrection.center().x() * 2.0, radialCorrection.center().y() * 2.0, 300);
    unsetCursor();
    LensDistortionModelParametersControlWidget::setParameters(inverted.mParams);
}

void LensDistortionModelParametersControlWidget::compareCorrection()
{
    QString filename =  QFileDialog::getOpenFileName(
                this,
                "Choose an file to compare with",
                ".",
                "json distortion params (*.json)"
                );

    if (!filename.isEmpty())
    {
        JSONGetter getter(filename);
        LensDistortionModelParameters otherLensParams;
        otherLensParams.accept(getter);
        RadialCorrection otherRadialCorrection(otherLensParams);


        LensDistortionModelParameters thisLensParams = LensDistortionModelParametersControlWidget::getParameters();
        RadialCorrection thisRadialCorrection(thisLensParams);

        EllipticalApproximation1d diff = thisRadialCorrection.compareWith(otherRadialCorrection, thisRadialCorrection.center().x() * 2.0, thisRadialCorrection.center().y() * 2.0, 300 );

        QMessageBox::information(this, "Comparison result",
                                 QString("Avg:%1 Max:%2").arg(diff.getRadiusAround0()).arg(diff.getMax()),
                                 QMessageBox::Ok
                                 );
    }



}

void LensDistortionModelParametersControlWidget::updateAdditionalDataNeeded()
{
    if (ui->autoRefeshCheckBox->isChecked()){
        updateAdditionalData();
    }
}


/* Additional stuff */
void LensDistortionModelParametersControlWidget::updateAdditionalData()
{
    setCursor(Qt::BusyCursor);

    LensDistortionModelParameters lensParams = LensDistortionModelParametersControlWidget::getParameters();
    RadialCorrection radialCorrection(lensParams);

    Vector2dd principal(lensParams.principalX(), lensParams.principalY());

    int diagonal = sqrt(principal.l2Metric());
    if (mExample != NULL)
    {
        diagonal = Vector2dd(mExample->width(), mExample->height()).l2Metric();
    }

    for (int i = 0; i < 20; i++)
    {
        mGraphDialog.addGraphPoint(0, 0, false);
    }

    for (int i = diagonal - 1; i >=0; i--)
    {
        mGraphDialog.addGraphPoint(0, lensParams.radialScale((double)i), true);
    }
    mGraphDialog.update();

    PrinterVisitor printer;
    printer.visit(lensParams, LensDistortionModelParameters(), "inputPrams");


    if (mExample != NULL)
    {
        qDebug() << "LensDistortionModelParametersContolWidget::updateAdditionalData(): Reprocessing...";
        /* OK... */
        delete_safe(mInput);
        delete_safe(mCorrected);
        delete_safe(mInverse);
        delete_safe(mBackproject);
        delete_safe(mDiff);
        delete_safe(mIsolines);

        mInput = QTFileLoader::RGB24BufferFromQImage(mExample);

        double step = ui->freeParamSpinBox->value();
        if (step < 0.05)
        {
            step = 0.05;
        }

        DisplacementBuffer* mDistortionCorrectTransform =
                 DisplacementBuffer::CacheInverse(&radialCorrection,
                 mInput->h, mInput->w,
                 0.0,0.0,
                 (double)mInput->w, (double)mInput->h,
                 step, 0
        );

        //DisplacementBuffer* mDistortionCorrectTransform = DisplacementBuffer::TestWiggle(mInput->h, mInput->w);

        if (!ui->bilinearCheckBox->isChecked()) {
            mCorrected = mInput->doReverseDeformation<RGB24Buffer, DisplacementBuffer>(*mDistortionCorrectTransform);
        } else {
            FixedPointDisplace displace(*mDistortionCorrectTransform, mDistortionCorrectTransform->h, mDistortionCorrectTransform->w);
            mCorrected = mInput->doReverseDeformationBlPrecomp(&displace);
        }

        mInverse     = mInput    ->doReverseDeformation<RGB24Buffer, RadialCorrection  >(radialCorrection);
        mBackproject = mCorrected->doReverseDeformation<RGB24Buffer, RadialCorrection  >(radialCorrection);

        Map2DFunction<DisplacementBuffer> mapFunc(mDistortionCorrectTransform);
        LengthFunction lengthFunc(&mapFunc);

        mIsolines = new RGB24Buffer(mDistortionCorrectTransform->getSize());
        mIsolines->drawIsolines(0.0, 0.0,
                (double)mDistortionCorrectTransform->h,  (double)mDistortionCorrectTransform->w,
                0.1, lengthFunc);

        double sum = 0;
        mDiff = new RGB24Buffer(mInput->getSize());
        for (int i = 0; i < mInput->h; i++)
        {
            for (int j = 0; j < mInput->w; j++)
            {
                RGBColor diff = RGBColor::diff(mInput->element(i,j),mBackproject->element(i,j));
                mDiff->element(i,j) = diff;
                sum += diff.luma();
            }
        }
        sum /= mInput->h * mInput->w;
        AbstractPainter<RGB24Buffer>(mDiff).drawFormat(20, 20, RGBColor::Indigo(), 1, "mean diff: %f", sum);


        delete_safe(mDistortionCorrectTransform);

        /* FINAL */

        exampleShow();
    }

    unsetCursor();

}


void LensDistortionModelParametersControlWidget::loadExample()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        "Choose an file name",
        ".",
        "Image (*.jpg *.jpeg *.bmp *.pgm *.png)"
        );


    QImage *image = new QImage(filename);
    if (image->isNull())
    {
        delete image;
        return;
    }

    delete_safe(mExample);
    mExample = image;
    updateAdditionalData();
}

void LensDistortionModelParametersControlWidget::showGraphDialog()
{
    mGraphDialog.show();
}

void LensDistortionModelParametersControlWidget::loadParams()
{
    qDebug() << "LensDistortionModelParametersControlWidget::loadParams(): called";
    QString filename = QFileDialog::getOpenFileName(
                this,
                "Choose an file name",
                ".",
                "json distortion params (*.json)"
                );

    if (!filename.isEmpty())
    {
        JSONGetter getter(filename);
        WidgetLoader loader(&getter);
        loadParamWidget(loader);
    }
}

void LensDistortionModelParametersControlWidget::saveParams()
{
    qDebug() << "LensDistortionModelParametersControlWidget::saveParams(): called";
    QString filename = QFileDialog::getSaveFileName(
                this,
                "Choose an file name",
                ".",
                "json distortion params (*.json)"
                );

    {
        JSONSetter setter(filename);
        WidgetSaver saver(&setter);
        saveParamWidget(saver);
    }
}

void LensDistortionModelParametersControlWidget::toggleAdvanced(bool flag)
{
    ui->bottomToolWidget->setHidden(!flag);
    ui->sideToolWidget  ->setHidden(!flag);
}

void LensDistortionModelParametersControlWidget::exampleShow()
{
    if (mExample == NULL)
    {
        return;
    }
    switch (ui->exampleComboBox->currentIndex()) {
    case 0:
        mDemoDialog.setImage(QSharedPointer<QImage>(new QImage(*mExample)));
        mDemoDialog.setWindowTitle("Example Input Image");
        break;
    case 1:
        qDebug() << "Outputing corrected";
        mDemoDialog.setImage(QSharedPointer<QImage>(new RGB24Image(mCorrected)));
        mDemoDialog.setWindowTitle("Corrected Output");
        break;
    case 2:
        qDebug() << "Outputing inverse";
        mDemoDialog.setImage(QSharedPointer<QImage>(new RGB24Image(mInverse)));
        mDemoDialog.setWindowTitle("Inverse Corrected");
        break;
    case 3:
        qDebug() << "Outputing backproject";
        mDemoDialog.setImage(QSharedPointer<QImage>(new RGB24Image(mBackproject)));
        mDemoDialog.setWindowTitle("Backproject Output");
        break;
    case 4:
        qDebug() << "Outputing diff";
        mDemoDialog.setImage(QSharedPointer<QImage>(new RGB24Image(mDiff)));
        mDemoDialog.setWindowTitle("Difference");
        break;
    case 5:
        qDebug() << "Outputing isolines";
        mDemoDialog.setImage(QSharedPointer<QImage>(new RGB24Image(mIsolines)));
        mDemoDialog.setWindowTitle("Isolines view");
        break;
    default:
        break;
    }
    mDemoDialog.show();
}
