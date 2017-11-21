#ifndef SEMIGLOBALBLOCKMATCHING_H
#define SEMIGLOBALBLOCKMATCHING_H

#include "opencv2/calib3d/calib3d.hpp"

#include "core/buffers/g12Buffer.h"
#include "core/buffers/flow/flowBuffer.h"
#include "core/math/vector/vector2d.h"

#include "openCVSGMParameters.h"
#include "openCVBMParameters.h"

using namespace cv;

using namespace corecvs;


class BMOpenCV
{
public:

#ifdef WITH_OPENCV_3x
    cv::Ptr< StereoBM > bm;
#else
    StereoBM bm;
#endif

    FlowBuffer* getStereoBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVBMParameters &params);
};


class SGBMOpenCV
{
public:

#ifdef WITH_OPENCV_3x
    cv::Ptr< StereoSGBM > sgbm;
#else
    StereoSGBM sgbm;
#endif

    FlowBuffer* getStereoSGBM(
            G12Buffer *InputBufferLeft,
            G12Buffer *InputBufferRight,
            const OpenCVSGMParameters &params);
};

#endif // SEMIGLOBALBLOCKMATCHING_H
