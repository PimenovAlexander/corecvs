//
// Created by jakhremchik
//

#include "apriltagDetector.h"

//apriltag_family_t *tf = NULL;
//tf = tag36h11_create();
//
//apriltag_detection_t *td = apriltag_detector_create();
//apriltag_detector_add_family(td, tf);
//td->quad_decimate =
apriltagDetector::apriltagDetector() {

}

apriltagDetector::~apriltagDetector() {
    delete_safe(debug);

}

bool apriltagDetector::setParameters(std::string name, const corecvs::DynamicObject &param) {
    if (name == "") {
        param.copyTo();
        return  true;
    }

}
