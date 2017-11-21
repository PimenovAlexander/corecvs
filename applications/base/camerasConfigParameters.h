#pragma once

#include <QtCore/QRect>
#include <QtCore/QSharedPointer>

#include "core/utils/global.h"

#include "universalRectifier.h"
#include "core/math/matrix/matrix33.h"
#include "core/alignment/distortionCorrectTransform.h"     // "../../core/src/alignment"
#include "core/buffers/displacementBuffer.h"


class CamerasConfigParameters
{
public:
    enum CaptureDevicesNum
    {
        OneCapDev = 1,
        TwoCapDev = 2
    };

    CamerasConfigParameters()
    {}

    Matrix33 leftPretransform() const;
    void setLeftPretransform(Matrix33 const &transform);
    Matrix33 rightPretransform() const;
    void setRightPretransform(Matrix33 const &transform);

    RectificationResult rectifierData() const;
    void setRectifierData(RectificationResult const & data);


    CaptureDevicesNum inputsN() const;
    void setInputsN(CaptureDevicesNum const inputs);

    void setDistortionTransform(QSharedPointer<DisplacementBuffer> transform);
    QSharedPointer<DisplacementBuffer> distortionTransform();

template<class VisitorType>
    void accept(VisitorType &visitor)
    {
        visitor.visit(mRectifierData, RectificationResult(), "rectification");
        visitor.visit(mInputsN, 0, "numInputs");
    }

protected:
    RectificationResult mRectifierData;
    CaptureDevicesNum mInputsN;
    QSharedPointer<DisplacementBuffer> mDistortionTransform;
};

