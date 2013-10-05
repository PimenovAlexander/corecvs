#ifndef SEMIGLOBALBLOCKMATCHING_H
#define SEMIGLOBALBLOCKMATCHING_H

#include "opencv2/calib3d/calib3d.hpp"

#include "g12Buffer.h"
#include "flowBuffer.h"
#include "vector2d.h"

#include "openCVSGMParameters.h"
#include "openCVBMParameters.h"

using namespace cv;

using namespace corecvs;


class BMOpenCV
{
public:

    StereoBM bm;

    FlowBuffer* getStereoBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVBMParameters &params);
};


class SGBMOpenCV
{
public:

    StereoSGBM sgbm;

    FlowBuffer* getStereoSGBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVSGMParameters &params);
};

#endif // SEMIGLOBALBLOCKMATCHING_H
