#pragma once

#include "core/math/vector/vector2d.h"
#include "core/math/vector/vector3d.h"

#include "core/patterndetection/patternDetector.h"
#include "core/patterndetection/boardAligner.h"
#include "core/xml/generated/checkerboardDetectionParameters.h"

#include <opencv2/core/core.hpp>        // Point2f


class OpenCvCheckerboardDetector : public PatternDetector
                                 , protected CheckerboardDetectionParameters
                                 , protected BoardAligner
{
public:
    OpenCvCheckerboardDetector(const CheckerboardDetectionParameters &params = CheckerboardDetectionParameters()
        , BoardAlignerParams boardAlignerParams = BoardAlignerParams());

    virtual bool detectPattern(corecvs::G8Buffer &buffer);

    using PatternDetector::getPointData;

    virtual void getPointData(corecvs::ObservationList &observations);

private:
    bool detectChessBoardOpenCv(corecvs::G8Buffer &buffer);
    void assignPointCoordinates(corecvs::G8Buffer &buffer);

    std::vector<cv::Point2f> points;
    int bw, bh;
    corecvs::ObservationList result;
};
