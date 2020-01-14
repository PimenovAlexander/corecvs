#ifndef OPENCV_CHECKER_BOARD_DETECTOR_H
#define OPENCV_CHECKER_BOARD_DETECTOR_H

#include <vector>

#include <core/patterndetection/patternDetector.h>

#include <core/stats/calculationStats.h>
#include <core/buffers/rgb24/rgb24Buffer.h>
#include <opencv2/core/mat.hpp>
#include "../generated/openCVCheckerBoardDetectorParameters.h"


class OpenCVCheckerBoardDetector : public corecvs::PatternDetector
{
public:
    corecvs::Statistics *stats = NULL;

    cv::Mat input;
     std::vector<cv::Point2f> corners;

    /** Parameters */
    OpenCVCheckerBoardDetectorParameters params;

    OpenCVCheckerBoardDetector();
    virtual ~OpenCVCheckerBoardDetector();

    /** DebuggableBlock interface */
public:
    corecvs::RGB24Buffer *debugBuffer = NULL;
    std::vector<std::string> debugBuffers() const override;
    corecvs::RGB24Buffer *getDebugBuffer(const std::string &name) const override;

    /** NewStyleBlock interface */
public:
    int operator () () override;

    /** PatternDetector interface */
public:
    std::map<std::string, corecvs::DynamicObject> getParameters() override;
    bool setParameters(std::string name, const corecvs::DynamicObject &param) override;

    void setInputImage(corecvs::RGB24Buffer *input) override;

    void getOutput(vector<corecvs::PatternDetectorResult> &patterns) override;

    void setStatistics(corecvs::Statistics *stats) override {
        this->stats = stats;
    }
};




#endif // OPENCV_CHECKER_BOARD_DETECTOR_H
