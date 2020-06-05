#pragma once

#include "math/vector/vector2d.h"
#include "math/vector/vector3d.h"

#include "patterndetection/patternDetector.h"
#include "patterndetection/boardAligner.h"
#include "xml/generated/checkerboardDetectionParameters.h"


#include <opencv2/core/core.hpp>        // Point2f

class OpenCvCheckerboardDetector : public corecvs::PatternGeometryDetector
                                 , protected CheckerboardDetectionParameters
                                 , protected BoardAligner
{
public:
    OpenCvCheckerboardDetector(const CheckerboardDetectionParameters &params = CheckerboardDetectionParameters()
        , BoardAlignerParams boardAlignerParams = BoardAlignerParams());

    virtual bool detectPattern(corecvs::G8Buffer &buffer);

    using corecvs::PatternGeometryDetector::getPointData;

    virtual void getPointData(corecvs::ObservationList &observations);

private:
    bool detectChessBoardOpenCv(corecvs::G8Buffer &buffer);
    void assignPointCoordinates(corecvs::G8Buffer &buffer);

    std::vector<cv::Point2f> points;
    int bw, bh;
    corecvs::ObservationList result;
};
