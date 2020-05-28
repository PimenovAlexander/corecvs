#include "stereointerface/processor6D.h"

namespace corecvs {

int Processor6D::setFrameRGB24(int frameType, RGB24Buffer *frame) {
    return setFrameRGB24(frame, frameType);
}

int Processor6D::setFrameG12(int frameType, G12Buffer *frame) {
    return setFrameG12  (frame, frameType);
}

} //namespace
