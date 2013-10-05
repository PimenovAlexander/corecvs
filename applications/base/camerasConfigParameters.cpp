#include "camerasConfigParameters.h"

Matrix33 CamerasConfigParameters::leftPretransform() const
{
   return mRectifierData.leftTransform;
}

void CamerasConfigParameters::setLeftPretransform(Matrix33 const &transform)
{
    mRectifierData.leftTransform = transform;
}

Matrix33 CamerasConfigParameters::rightPretransform() const
{
    return mRectifierData.rightTransform;
}

void CamerasConfigParameters::setRightPretransform(Matrix33 const &transform)
{
    mRectifierData.rightTransform = transform;
}


RectificationResult CamerasConfigParameters::rectifierData() const
{
    return mRectifierData;
}

void CamerasConfigParameters::setRectifierData(RectificationResult const & data)
{
    mRectifierData = data;
}


CamerasConfigParameters::CaptureDevicesNum CamerasConfigParameters::inputsN() const
{
    return mInputsN;
}

void CamerasConfigParameters::setInputsN(CamerasConfigParameters::CaptureDevicesNum const inputs)
{
    mInputsN = inputs;
}

void CamerasConfigParameters::setDistortionTransform(QSharedPointer<DisplacementBuffer> transform)
{
    mDistortionTransform = transform;
}

QSharedPointer<DisplacementBuffer> CamerasConfigParameters::distortionTransform()
{
    return mDistortionTransform;
}
