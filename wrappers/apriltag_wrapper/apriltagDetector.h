//
// Created by jakhremchik
//

#ifndef CORECVS_APRILTAGDETECTOR_H
#define CORECVS_APRILTAGDETECTOR_H

#include <core/patterndetection/patternDetector.h>
#include "opencv2/opencv.hpp"
#include <core/stats/calculationStats.h>

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

class apriltagDetector : public corecvs::PatternDetector
{
public:
    //TODO It's temporary. Change it with reflection.
    struct apriltagParametrs {
        std::string tag_family;
        int at_threads;
        bool at_debug;
        bool at_quiet;
        bool at_refine_edges;
        double at_decimate;
        double blur;
    };

    apriltagParametrs params;

    corecvs::Statistics *stats = NULL;

    cv::Mat gray; //probably replace with native input later

//    corecvs::G8Buffer *gray = NULL;

    corecvs::RGB24Buffer *debug = NULL;

    std::vector<corecvs::PatternDetectorResult> apriltagResult;

    apriltagDetector() ;
    virtual  ~apriltagDetector();

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

    void setInputImageCV(const cv::Mat& input);

    void getOutput(vector<corecvs::PatternDetectorResult> &patterns) override;

    void setStatistics(corecvs::Statistics *stats) override {
        this->stats = stats;
    }

private:
    apriltag_detector_t *td;
    zarray *at_detections;
};

#endif //CORECVS_APRILTAGDETECTOR_H
