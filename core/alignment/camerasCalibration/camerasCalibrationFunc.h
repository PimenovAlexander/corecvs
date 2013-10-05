#pragma once
#include "function.h"

namespace corecvs {

class CamerasCalibrationFunc : public FunctionArgs
{
public:
    CamerasCalibrationFunc(const vector<Vector3dd> *sample, Vector2d32 const &center);
    virtual void operator()(const double in[], double out[]);
    virtual Matrix getJacobian(const double in[], double delta = 1e-15);
    void setIsDistored(bool isDistored);
    void setIsFocalLengthConst(bool isFocalLengthConst);
    void setIsOriginConst(bool isOriginConst);

private:
    const vector<Vector3dd> *mSample;
    Vector2d32 mCenter;
    bool mIsDistored;
    bool mIsFocalLengthConst;
    bool mIsOriginConst;
};
}
