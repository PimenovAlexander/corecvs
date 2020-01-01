#ifndef OPENCVSQUAREDETECTOR_H
#define OPENCVSQUAREDETECTOR_H

#include <vector>

#include <patterndetection/patternDetector.h>

#include <stats/calculationStats.h>
#include <buffers/rgb24/rgb24Buffer.h>
#include <opencv2/core/mat.hpp>
#include "generated/openCVSquareDetectorParameters.h"


class OpenCVSquareDetector : public corecvs::PatternDetector
{
public:
    corecvs::Statistics *stats = NULL;

    cv::Mat input;
    std::vector<std::vector<cv::Point> > squares;

    /** Parameters */
    OpenCVSquareDetectorParameters params;

    OpenCVSquareDetector();
    virtual ~OpenCVSquareDetector();

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




#endif // OPENCVSQUAREDETECTOR_H
