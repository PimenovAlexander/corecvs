#pragma once

#include "vector2d.h"
#include "vector3d.h"

#include "patternDetector.h"
#include "boardAligner.h"
#include "checkerboardDetectionParameters.h"

#include <opencv2/core/core.hpp>        // cv::Mat, Point2f


class OpenCvCheckerboardDetector : public PatternDetector, protected CheckerboardDetectionParameters, protected BoardAligner
{
public:
    OpenCvCheckerboardDetector(const CheckerboardDetectionParameters &params = CheckerboardDetectionParameters(), BoardAlignerParams boardAlignerParams = BoardAlignerParams());

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
