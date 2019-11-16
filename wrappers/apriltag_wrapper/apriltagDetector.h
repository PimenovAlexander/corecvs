//
// Created by jakhremchik
//

#ifndef CORECVS_APRILTAGDETECTOR_H
#define CORECVS_APRILTAGDETECTOR_H

#include <core/patterndetection/patternDetector.h>
#include "opencv2/opencv.hpp"
#include <core/stats/calculationStats.h>
#include "generated/apriltagParameters.h"

extern "C" {
#include "apriltag/apriltag.h"
#include "apriltag/tag36h11.h"
#include "apriltag/tag16h5.h"
#include "apriltag/tag25h9.h"
#include "apriltag/tagCircle21h7.h"
#include "apriltag/tagCircle49h12.h"
#include "apriltag/tagStandard41h12.h"
#include "apriltag/tagStandard52h13.h"
#include "apriltag/tagCustom48h12.h"
#include "apriltag/common/getopt.h"
}

class ApriltagDetector : public corecvs::PatternDetector
{
public:

    ApriltagParameters params;

    corecvs::Statistics *stats = nullptr;

    corecvs::G8Buffer *gray = nullptr;

    corecvs::RGB24Buffer *debugBuffer = nullptr;
    corecvs::RGB24Buffer *input = nullptr;

    ApriltagDetector() ;
    virtual  ~ApriltagDetector();

    /** DebuggableBlock interface */
public:
    std::vector<std::string> debugBuffers() const override;
    corecvs::RGB24Buffer *getDebugBuffer(const std::string &name) const override;

public:
    int operator () () override;

public:
    std::map<std::string, corecvs::DynamicObject> getParameters() override;
    bool setParameters(std::string name, const corecvs::DynamicObject &param) override;

    void setInputImage(corecvs::RGB24Buffer *input) override;

    void getOutput(vector<corecvs::PatternDetectorResult> &patterns) override;

    void setStatistics(corecvs::Statistics *stats) override {
        this->stats = stats;
    }

private:
    apriltag_detector_t *td;
    zarray *at_detections;
};

#endif //CORECVS_APRILTAGDETECTOR_H
