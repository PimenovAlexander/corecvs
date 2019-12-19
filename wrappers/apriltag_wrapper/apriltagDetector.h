#ifndef CORECVS_APRILTAGDETECTOR_H
#define CORECVS_APRILTAGDETECTOR_H

/**
 * \file apriltagDetector.h
 * \brief a header for apriltagDetector.cpp
 *
 * \date Nov 27, 2019
 * \author jakhremchik
 **/


#include "core/patterndetection/patternDetector.h"
#include "core/stats/calculationStats.h"
#include "generated/apriltagParameters.h"

extern "C" {
struct  apriltag_detector;
typedef apriltag_detector apriltag_detector_t;
struct  apriltag_family;
typedef apriltag_family apriltag_family_t;
struct zarray;
typedef zarray zarray_t;
struct apriltag_quad_thresh_params;
typedef apriltag_quad_thresh_params apriltag_quad_thresh_params_t;

}

class ApriltagDetector : public corecvs::PatternDetector
{
public:

    ApriltagParameters params;

//    April

    corecvs::Statistics *stats = nullptr;

    corecvs::G8Buffer *gray = nullptr;

    corecvs::RGB24Buffer *debugBuffer = nullptr;
    corecvs::RGB24Buffer *input = nullptr;

    ApriltagDetector();
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
    bool apriltag_create();
    bool apriltag_destroy();

    apriltag_family_t *tf   = nullptr;
    apriltag_detector_t *td = nullptr;
    zarray_t *at_detections = nullptr;
    apriltag_quad_thresh_params *qtp = nullptr;
};

#endif //CORECVS_APRILTAGDETECTOR_H
