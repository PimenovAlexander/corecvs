/**
 * \file headSearchParametersControlWidget.cpp
 * \attention This file is automatically generated and should not be in general modified manually
 *
 * \date MMM DD, 20YY
 * \author autoGenerator
 */

#include "headSearchParametersControlWidget.h"
#include "ui_headSearchParametersControlWidget.h"
#include <memory>
#include "visitors/qSettingsGetter.h"
#include "visitors/qSettingsSetter.h"


HeadSearchParametersControlWidget::HeadSearchParametersControlWidget(QWidget *parent, bool _autoInit, QString _rootPath)
    : ParametersControlWidgetBase(parent)
    , mUi(new Ui::HeadSearchParametersControlWidget)
    , autoInit(_autoInit)
    , rootPath(_rootPath)
{
    mUi->setupUi(this);

    QObject::connect(mUi->thresholdDistanceSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->clusterDepthSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->clusterMinSizeSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->headAreaRadiusSpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(paramsChanged()));
    QObject::connect(mUi->headNumberSpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(paramsChanged()));
}

HeadSearchParametersControlWidget::~HeadSearchParametersControlWidget()
{

    delete mUi;
}

void HeadSearchParametersControlWidget::loadParamWidget(WidgetLoader &loader)
{
    std::unique_ptr<HeadSearchParameters> params(createParameters());
    loader.loadParameters(*params, rootPath);
    setParameters(*params);
}

void HeadSearchParametersControlWidget::saveParamWidget(WidgetSaver  &saver)
{
    saver.saveParameters(*std::unique_ptr<HeadSearchParameters>(createParameters()), rootPath);
}

void HeadSearchParametersControlWidget::getParameters(HeadSearchParameters& params) const
{
    params = *std::unique_ptr<HeadSearchParameters>(createParameters());
}


HeadSearchParameters *HeadSearchParametersControlWidget::createParameters() const
{

    /**
     * We should think of returning parameters by value or saving them in a preallocated place
     **/


    return new HeadSearchParameters(
          mUi->thresholdDistanceSpinBox->value()
        , mUi->clusterDepthSpinBox->value()
        , mUi->clusterMinSizeSpinBox->value()
        , mUi->headAreaRadiusSpinBox->value()
        , mUi->headNumberSpinBox->value()
    );
}

void HeadSearchParametersControlWidget::setParameters(const HeadSearchParameters &input)
{
    // Block signals to send them all at once
    bool wasBlocked = blockSignals(true);
    mUi->thresholdDistanceSpinBox->setValue(input.thresholdDistance());
    mUi->clusterDepthSpinBox->setValue(input.clusterDepth());
    mUi->clusterMinSizeSpinBox->setValue(input.clusterMinSize());
    mUi->headAreaRadiusSpinBox->setValue(input.headAreaRadius());
    mUi->headNumberSpinBox->setValue(input.headNumber());
    blockSignals(wasBlocked);
    emit paramsChanged();
}

void HeadSearchParametersControlWidget::setParametersVirtual(void *input)
{
    // Modify widget parameters from outside
    HeadSearchParameters *inputCasted = static_cast<HeadSearchParameters *>(input);
    setParameters(*inputCasted);
}
