//
// Created by jakhremchik
//

#ifndef CORECVS_APRILTAGDETECTOR_H
#define CORECVS_APRILTAGDETECTOR_H

#include <core/patterndetection/patternDetector.h>
#include "opencv2/opencv.hpp"

extern "C" {
#include "apriltag/apriltag.h"
#include "apriltag/tag36h11.h"
#include "apriltag/common/getopt.h"
}

class apriltagDetector : public corecvs::PatternDetector
{
    corecvs::Statistics *stats = NULL;
    cv::Mat input; //probably replace with native input later

    corecvs::RGB24Buffer *debug = NULL;

    std::vector<corecvs::PatternDetectorResult> apriltagResult;

    apriltagDetector();
    virtual  ~apriltagDetector();

    /** DebuggableBlock interface */
public:
    std::vector<std::string> debugBuffers() const override;
    corecvs::RGB24Buffer *getDebugBuffer(const std::string &name) const override;

public:
    int operator () () override;

public:
    std::map<std::string, corecvs::DynamicObject> getParameters override;
    bool setParameters(std::string name, const corecvs::DynamicObject &param) override;

    void setInputImage(corecvs::RGB24Buffer *input) override;

    void getOutput(vector<corecvs::PatternDetectorResult> &patterns) override;

    void setStatistics(corecvs::Statistics *stats) override {
        this-stats = stats;
    }

};

#endif //CORECVS_APRILTAGDETECTOR_H
