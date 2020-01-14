#ifndef DUMMYPATTERNDETECTOR_H
#define DUMMYPATTERNDETECTOR_H

#include "core/patterndetection/patternDetector.h"

namespace corecvs {

class DummyPatternDetector : public PatternDetector
{
public:
    Statistics *stats = NULL;
    RGB24Buffer *input = NULL;

    RGB24Buffer *debug = NULL;


    PatternDetectorResultBase dummyResult;
    RgbColorParameters color;

    DummyPatternDetector();
    virtual ~DummyPatternDetector();

    /** DebuggableBlock interface */
public:
    std::vector<std::string> debugBuffers() const override;
    RGB24Buffer *getDebugBuffer(const std::string &name) const override;

    /** NewStyleBlock interface */
public:
    int operator () () override {
        Statistics::startInterval(stats);
        SYNC_PRINT(("DummyPatternDetector::operator(): called\n"));
        Statistics::endInterval(stats, "operator()");
        return 0;
    }

    /** PatternDetector interface */
public:
    std::map<std::string, DynamicObject> getParameters() override;
    bool setParameters(std::string name, const DynamicObject &param) override;

    void setInputImage(RGB24Buffer *input) override{
        Statistics::startInterval(stats);
        this->input = input;
        Statistics::endInterval(stats, "Setting input");
    }

    void getOutput(vector<PatternDetectorResult> &patterns) override;

    void setStatistics(Statistics *stats) override {
        this->stats = stats;
    }
};




} // namespace corecvs

#endif // DUMMYPATTERNDETECTOR_H
