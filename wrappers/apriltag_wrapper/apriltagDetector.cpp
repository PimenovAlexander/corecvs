//
// Created by jakhremchik
//

#include "apriltagDetector.h"

extern "C" {
#if 0
#include <apriltag/apriltag.h>
#include <apriltag/tag36h11.h>
#include <apriltag/tag16h5.h>
#include <apriltag/tag25h9.h>
#include <apriltag/tagCircle21h7.h>
#include <apriltag/tagCircle49h12.h>
#include <apriltag/tagStandard41h12.h>
#include <apriltag/tagStandard52h13.h>
#include <apriltag/tagCustom48h12.h>
#include <apriltag/common/getopt.h>
#else
#include <apriltag.h>
#include <tag36h11.h>
#include <tag16h5.h>
#include <tag25h9.h>
#include <tagCircle21h7.h>
#include <tagCircle49h12.h>
#include <tagStandard41h12.h>
#include <tagStandard52h13.h>
#include <tagCustom48h12.h>
#include <common/getopt.h>
#endif
}

ApriltagDetector::ApriltagDetector(){
    at_detections = nullptr;
    apriltag_create();
}

ApriltagDetector::~ApriltagDetector() {
    apriltag_destroy();
}

int ApriltagDetector::operator()() {
    using namespace corecvs;
    SYNC_PRINT(("apriltagDetector::operator(): called\n"));

    image_u8_t inputGray = {
            .width  = gray->w,
            .height = gray->h,
            .stride = gray->stride,
            .buf   = gray->data
    };


    Statistics::startInterval(stats);
    at_detections = apriltag_detector_detect(this->td, &inputGray);

    Statistics::endInterval(stats, "Finalize");
    return 0;
}

bool ApriltagDetector::setParameters(std::string name, const corecvs::DynamicObject &param) {
    if ("params" == name) {
        apriltag_destroy();
        param.copyTo(&params);
        apriltag_create();
        return  true;
    }
    return false;

}

void ApriltagDetector::setInputImage(corecvs::RGB24Buffer *input) {
    corecvs::Statistics::startInterval(stats);
    delete_safe(gray);
    this->input = input;
    gray = input->getChannelG8(ImageChannel::GRAY);

    corecvs::Statistics::endInterval(stats, "Setting input");
}


void ApriltagDetector::getOutput(vector<corecvs::PatternDetectorResult> &patterns) {
    using namespace corecvs;
    Statistics::startInterval(stats);
    for (int i = 0; i < zarray_size(at_detections); i++) {
        PatternDetectorResult result;
        apriltag_detection *det;
        zarray_get(at_detections, i, &det);
        result.mPosition = Vector2dParameters(det->p[0][0], det->p[0][1]);
        result.mOrtX = Vector2dParameters(det->p[1][0], det->p[1][1]);
        result.mOrtY = Vector2dParameters(det->p[3][0], det->p[3][1]);
        result.mUnityPoint = Vector2dParameters(det->p[2][0], det->p[2][1]);
        result.mId = det->id;
        patterns.emplace_back(result);
    }
    if (params.at_debug()) {
        delete_safe(debugBuffer);
        debugBuffer = new RGB24Buffer(input);
        for (const auto &result : patterns) {
            Vector2dd d (result.mPosition.mX, result.mPosition.mY);
            Vector2dd c (result.mOrtX.mX, result.mOrtX.mY);
            Vector2dd a (result.mOrtY.mX, result.mOrtY.mY);
            Vector2dd b (result.mUnityPoint.mX, result.mUnityPoint.mY);

            RGBColor color = RGBColor::parula((double)result.mId/patterns.size());
            debugBuffer->drawLine(a, b, color);
            debugBuffer->drawLine(b, c, color);
            debugBuffer->drawLine(c, d, color);
            debugBuffer->drawLine(d, a, color);

            AbstractPainter<RGB24Buffer> p(debugBuffer);
            p.drawFormat((a.x() + c.x())/2, (a.y() + c.y())/2, color, 2, "%d", result.mId);
        }

    }

    Statistics::endInterval(stats, "Getting output");
}

std::vector<std::string> ApriltagDetector::debugBuffers() const {
    if (debugBuffer != nullptr )
        return std::vector<std::string>({"debug"});

    return std::vector<std::string>();
}

corecvs::RGB24Buffer *ApriltagDetector::getDebugBuffer(const std::string &name) const {
    if ("debug" == name) {
        return new corecvs::RGB24Buffer(debugBuffer);
    }
    return nullptr;
}

std::map<std::string, corecvs::DynamicObject> ApriltagDetector::getParameters() {
    std::map<std::string, corecvs::DynamicObject> dparams;
    dparams.emplace("params", corecvs::DynamicObject(&params));
    return dparams;
}

bool ApriltagDetector::apriltag_destroy() {
   apriltag_detector_destroy(td);

   if       (AprilTagType::TAG36H11 == params.tag_family()) {
        tag36h11_destroy(tf);
    } else if (AprilTagType::TAG25H9 == params.tag_family()) {
        tag25h9_destroy(tf);
    } else if (AprilTagType::TAG16H5 == params.tag_family()) {
        tag16h5_destroy(tf);
    } else if (AprilTagType::TAGCIRCLE21H7 == params.tag_family()) {
        tagCircle21h7_destroy(tf);
    } else if (AprilTagType::TAGCIRCLE49H12 == params.tag_family()) {
        tagCircle49h12_destroy(tf);
    } else if (AprilTagType::TAGSTANDARD41H12 == params.tag_family()) {
        tagStandard41h12_destroy(tf);
    } else if (AprilTagType::TAGSTANDARD52H13 == params.tag_family()) {
        tagStandard52h13_destroy(tf);
    } else if (AprilTagType::TAGCUSTOM48H12 == params.tag_family()) {
        tagCustom48h12_destroy(tf);
    } else {
       return false;
    }

    delete_safe(debugBuffer);
    delete_safe(gray);
    return true;
}

bool ApriltagDetector::apriltag_create() {
      /* Add case here*/
     if       (AprilTagType::TAG36H11 == params.tag_family()) {
        tf = tag36h11_create();
    } else if (AprilTagType::TAG25H9 == params.tag_family()) {
        tf = tag25h9_create();
    } else if (AprilTagType::TAG16H5 == params.tag_family()) {
        tf = tag16h5_create();
        // circles probably won't return correct output for PatternDetection.
    } else if (AprilTagType::TAGCIRCLE21H7 == params.tag_family()) {
        tf = tagCircle21h7_create();
    } else if (AprilTagType::TAGCIRCLE49H12 == params.tag_family()) {
        tf = tagCircle49h12_create();
    } else if (AprilTagType::TAGSTANDARD41H12 == params.tag_family()) {
        tf = tagStandard41h12_create();
    } else if (AprilTagType::TAGSTANDARD52H13 == params.tag_family()) {
        tf = tagStandard52h13_create();
    } else if (AprilTagType::TAGCUSTOM48H12 == params.tag_family()) {
        tf = tagCustom48h12_create();
    } else {
        printf("Unrecognized tag family name. Use e.g. \"tag36h11\".\n");
        exit(-1);
    }

    td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    td->debug         = params.at_debug();
    td->quad_decimate = params.at_decimate();
    td->quad_sigma    = params.blur();
    td->refine_edges  = params.at_refine_edges();
    td->nthreads      = params.at_threads();

    return true;
}


