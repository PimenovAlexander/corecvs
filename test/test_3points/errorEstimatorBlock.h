#ifndef ERRORESTIMATORBLOCK_H
#define ERRORESTIMATORBLOCK_H

#include "core/math/vector/vector3d.h"
//#include "core/filters/blocks/newStyleBlock.h"
#include "gen/errorEstimatorBlockBase.h"

class ErrorEstimatorBlock : public ErrorEstimatorBlockBase/*, public corecvs::NewStyleBlock*/
{
public:
    ErrorEstimatorBlock();


    virtual int operator ()();

    ~ErrorEstimatorBlock() {
    }

    /** Somthing additional **/
    vector<corecvs::Vector3dd> targetScene(const Error3PointParameters &eParams);
};

#endif // ERRORESTIMATORBLOCK_H
