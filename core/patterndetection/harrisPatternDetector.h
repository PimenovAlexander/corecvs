#ifndef HARRISPATTERNDETECTOR_H
#define HARRISPATTERNDETECTOR_H

#include "patterndetection/patternDetector.h"

#include "xml/generated/harrisDetectionParameters.h"

namespace corecvs {

class HarrisPatternDetector : public PatternDetector
{
public:
    Statistics *stats = NULL;
    RGB24Buffer *input = NULL;

    G12Buffer *gray       = NULL;
    G12Buffer *highlights = NULL;

    HarrisDetectionParameters params;

    HarrisPatternDetector();

    // DebuggableBlock interface
public:
    std::vector<std::string> debugBuffers() const;
    RGB24Buffer *getDebugBuffer(const std::string &name) const;

    // NewStyleBlock interface
public:
    int operator ()() override;

    // PatternDetector interface
public:
    std::map<std::string, DynamicObject> getParameters() override;
    bool setParameters(std::string name, const DynamicObject &param) override;
    void setInputImage(RGB24Buffer *input) override;
    void getOutput(vector<PatternDetectorResult> &patterns) override;
    void setStatistics(Statistics *stats) override;
};

} // namespace corecvs

#endif // HARRISPATTERNDETECTOR_H
