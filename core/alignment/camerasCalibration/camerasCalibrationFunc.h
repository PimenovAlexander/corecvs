#pragma once
#include "core/function/function.h"

namespace corecvs {

class CamerasCalibrationFunc : public FunctionArgs
{
public:
    CamerasCalibrationFunc(const vector<Vector3dd> *sample, Vector2dd const &center);
    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);
    void setIsDistored(bool isDistored);
    void setIsFocalLengthConst(bool isFocalLengthConst);
    void setIsOriginConst(bool isOriginConst);

private:
    const vector<Vector3dd> *mSample;
    Vector2dd mCenter;
    bool mIsDistored;
    bool mIsFocalLengthConst;
    bool mIsOriginConst;
};
}
