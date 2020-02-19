#ifndef SEMIGLOBALBLOCKMATCHING_H
#define SEMIGLOBALBLOCKMATCHING_H

#include "opencv2/calib3d/calib3d.hpp"

#include "buffers/g12Buffer.h"
#include "buffers/flow/flowBuffer.h"
#include "math/vector/vector2d.h"

#include "generated/openCVSGMParameters.h"
#include "generated/openCVBMParameters.h"

using namespace cv;

using namespace corecvs;


class BMOpenCV
{
public:
    cv::Ptr< StereoBM > bm;

    FlowBuffer* getStereoBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVBMParameters &params);
};


class SGBMOpenCV
{
public:
    cv::Ptr< StereoSGBM > sgbm;

    FlowBuffer* getStereoSGBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVSGMParameters &params);
};

#endif // SEMIGLOBALBLOCKMATCHING_H
