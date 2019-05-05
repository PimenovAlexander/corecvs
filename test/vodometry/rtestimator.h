#ifndef RTESTIMATOR_H
#define RTESTIMATOR_H

#include "core/math/matrix/matrix33.h"
#include "core/buffers/flow/flowBuffer.h"

class RTEstimator
{
public:
    corecvs::Matrix33 R;
    corecvs::Vector3dd t;

    RTEstimator();

    void estimate(corecvs::Matrix33 E, corecvs::FlowBuffer *flow);
};

#endif // RTESTIMATOR_H
