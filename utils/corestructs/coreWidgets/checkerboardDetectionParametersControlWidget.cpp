/**
 * \file checkerboardDetectionParametersControlWidget.cpp
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include "checkerboardDetectionParametersControlWidget.h"
#include "ui_checkerboardDetectionParametersControlWidget.h"
#include <memory>
#include "visitors/qSettingsGetter.h"
#include "visitors/qSettingsSetter.h"


CheckerboardDetectionParametersControlWidget::CheckerboardDetectionParametersControlWidget(QWidget *parent, bool _autoInit, QString _rootPath)
    : ParametersControlWidgetBase(parent)
    , mUi(new Ui::CheckerboardDetectionParametersControlWidget)
    , autoInit(_autoInit)
    , rootPath(_rootPath)
{
    mUi->setupUi(this);

    QObject::connect(mUi->estimateUndistortedFromDistortedCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->useUndistortionCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->algorithmComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->channelComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->cellSizeHorSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->cellSizeVertSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->cleanExistingCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->preciseDiameterSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->iterationCountSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->minAccuracySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->partialBoardCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->fastBoardSpeedupCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->drawSGFsOnBoardsCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->skipUndistortedWithNoDistortedBoardCheckBox, SIGNAL(stateChanged(int)), this, SIGNAL(paramsChanged()));
}

CheckerboardDetectionParametersControlWidget::~CheckerboardDetectionParametersControlWidget()
{

    delete mUi;
}

void CheckerboardDetectionParametersControlWidget::loadParamWidget(WidgetLoader &loader)
{
    std::unique_ptr<CheckerboardDetectionParameters> params(createParameters());
    loader.loadParameters(*params, rootPath);
    setParameters(*params);
}

void CheckerboardDetectionParametersControlWidget::saveParamWidget(WidgetSaver  &saver)
{
    saver.saveParameters(*std::unique_ptr<CheckerboardDetectionParameters>(createParameters()), rootPath);
}

void CheckerboardDetectionParametersControlWidget::getParameters(CheckerboardDetectionParameters& params) const
{
    params = *std::unique_ptr<CheckerboardDetectionParameters>(createParameters());
}


CheckerboardDetectionParameters *CheckerboardDetectionParametersControlWidget::createParameters() const
{

    /**
     * We should think of returning parameters by value or saving them in a preallocated place
     **/


    return new CheckerboardDetectionParameters(
          mUi->estimateUndistortedFromDistortedCheckBox->isChecked()
        , mUi->useUndistortionCheckBox->isChecked()
        , static_cast<CheckerboardDetectionAlgorithm::CheckerboardDetectionAlgorithm>(mUi->algorithmComboBox->currentIndex())
        , static_cast<ImageChannel::ImageChannel>(mUi->channelComboBox->currentIndex())
        , mUi->cellSizeHorSpinBox->value()
        , mUi->cellSizeVertSpinBox->value()
        , mUi->cleanExistingCheckBox->isChecked()
        , mUi->preciseDiameterSpinBox->value()
        , mUi->iterationCountSpinBox->value()
        , mUi->minAccuracySpinBox->value()
        , mUi->partialBoardCheckBox->isChecked()
        , mUi->fastBoardSpeedupCheckBox->isChecked()
        , mUi->drawSGFsOnBoardsCheckBox->isChecked()
        , mUi->skipUndistortedWithNoDistortedBoardCheckBox->isChecked()
    );
}

void CheckerboardDetectionParametersControlWidget::setParameters(const CheckerboardDetectionParameters &input)
{
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);
    mUi->estimateUndistortedFromDistortedCheckBox->setChecked(input.estimateUndistortedFromDistorted());
    mUi->useUndistortionCheckBox->setChecked(input.useUndistortion());
    mUi->algorithmComboBox->setCurrentIndex(input.algorithm());
    mUi->channelComboBox->setCurrentIndex(input.channel());
    mUi->cellSizeHorSpinBox->setValue(input.cellSizeHor());
    mUi->cellSizeVertSpinBox->setValue(input.cellSizeVert());
    mUi->cleanExistingCheckBox->setChecked(input.cleanExisting());
    mUi->preciseDiameterSpinBox->setValue(input.preciseDiameter());
    mUi->iterationCountSpinBox->setValue(input.iterationCount());
    mUi->minAccuracySpinBox->setValue(input.minAccuracy());
    mUi->partialBoardCheckBox->setChecked(input.partialBoard());
    mUi->fastBoardSpeedupCheckBox->setChecked(input.fastBoardSpeedup());
    mUi->drawSGFsOnBoardsCheckBox->setChecked(input.drawSGFsOnBoards());
    mUi->skipUndistortedWithNoDistortedBoardCheckBox->setChecked(input.skipUndistortedWithNoDistortedBoard());
    blockSignals(wasBlocked);
    emit paramsChanged();
}

void CheckerboardDetectionParametersControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    CheckerboardDetectionParameters *inputCasted = static_cast<CheckerboardDetectionParameters *>(input);
    setParameters(*inputCasted);
}
