#ifndef OPENCVSQUAREDETECTOR_H
#define OPENCVSQUAREDETECTOR_H

#include <vector>

#include <core/patterndetection/patternDetector.h>

#include <core/stats/calculationStats.h>
#include <core/buffers/rgb24/rgb24Buffer.h>
#include <opencv2/core/mat.hpp>
#include "../generated/openCVSquareDetectorParameters.h"


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
    std::vector<std::string> debugBuffers() const;
    corecvs::RGB24Buffer *getDebugBuffer(const std::string &name) const;

    /** NewStyleBlock interface */
public:
    int operator () ();

    /** PatternDetector interface */
public:
    std::map<std::string, corecvs::DynamicObject> getParameters();
    bool setParameters(std::string name, const corecvs::DynamicObject &param);

    void setInputImage(corecvs::RGB24Buffer *input);

    void getOutput(vector<corecvs::Vector2dd> &patterns);

    void setStatistics(corecvs::Statistics *stats) {
        this->stats = stats;
    }
};




#endif // OPENCVSQUAREDETECTOR_H
