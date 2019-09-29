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


    Vector2dd point;
    RgbColorParameters color;

    DummyPatternDetector();
    virtual ~DummyPatternDetector();

    /** DebuggableBlock interface */
public:
    std::vector<std::string> debugBuffers() const;
    RGB24Buffer *getDebugBuffer(const std::string &name) const;

    /** NewStyleBlock interface */
public:
    int operator () (){
        Statistics::startInterval(stats);
        SYNC_PRINT(("DummyPatternDetector::operator(): called\n"));
        Statistics::endInterval(stats, "operator()");
        return 0;
    }

    /** PatternDetector interface */
public:
    std::map<std::string, DynamicObject> getParameters();
    bool setParameters(std::string name, const DynamicObject &param);

    void setInputImage(RGB24Buffer *input) {
        Statistics::startInterval(stats);
        this->input = input;
        Statistics::endInterval(stats, "Setting input");
    }

    void getOutput(vector<Vector2dd> &patterns);

    void setStatistics(Statistics *stats) {
        this->stats = stats;
    }
};




} // namespace corecvs

#endif // DUMMYPATTERNDETECTOR_H
