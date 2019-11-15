//
// Created by jakhremchik
//

#include "apriltagDetector.h"


ApriltagDetector::ApriltagDetector(){
    apriltag_family_t *tf = nullptr;
    at_detections = nullptr;

     if ("tag36h11" == params.tag_family()) {
        tf = tag36h11_create();
    } else if ("tag25h9" == params.tag_family()) {
        tf = tag25h9_create();
    } else if ("tag16h5" == params.tag_family()) {
        tf = tag16h5_create();
        // circles probably won't return correct output for PatternDetection.
    } else if ("tagCircle21h7" == params.tag_family()) {
        tf = tagCircle21h7_create();
    } else if ("tagCircle49h12" == params.tag_family()) {
        tf = tagCircle49h12_create();
    } else if ("tagStandard41h12" == params.tag_family()) {
        tf = tagStandard41h12_create();
    } else if ("tagStandard52h13" == params.tag_family()) {
        tf = tagStandard52h13_create();
    } else if ( "tagCustom48h12" == params.tag_family()) {
        tf = tagCustom48h12_create();
    } else {
        printf("Unrecognized tag family name. Use e.g. \"tag36h11\".\n");
        exit(-1);
    }

    this->td = apriltag_detector_create();
    apriltag_detector_add_family(td, tf);

    this->td->debug = params.at_debug();
    this->td->quad_decimate = params.at_decimate();
    this->td->quad_sigma = params.blur();
    this->td->refine_edges = params.at_refine_edges();
    this->td->nthreads = params.at_threads();
}

ApriltagDetector::~ApriltagDetector() {
    delete_safe(debugBuffer);
    delete_safe(gray);
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
}


bool ApriltagDetector::setParameters(std::string name, const corecvs::DynamicObject &param) {
    if ("params" == name) {
        param.copyTo(&params);
        return  true;
    }
    return false;

}

void ApriltagDetector::setInputImage(corecvs::RGB24Buffer *input) {
    corecvs::Statistics::startInterval(stats);
    delete_safe(gray);
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
            Vector2dd a (result.mPosition.mX, result.mPosition.mY);
            Vector2dd b (result.mOrtX.mX, result.mOrtX.mY);
            Vector2dd c (result.mOrtY.mX, result.mOrtY.mY);
            Vector2dd d (result.mUnityPoint.mX, result.mUnityPoint.mY);

            RGBColor color = RGBColor::parula((double)result.mId/patterns.size());
            debugBuffer->drawLine(a, b, color);
            debugBuffer->drawLine(b, c, color);
            debugBuffer->drawLine(c, d, color);
            debugBuffer->drawLine(d, a, color);

            AbstractPainter<RGB24Buffer> p(debugBuffer);
            p.drawFormat((result.mOrtX.mX + result.mOrtY.mX)/2, (result.mOrtX.mY + result.mOrtY.mX)/2, color, 2, "%d", result.mId);
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


