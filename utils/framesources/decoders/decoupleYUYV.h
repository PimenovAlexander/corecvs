#ifndef DECOUPLE_YUYV_H_
#define DECOUPLE_YUYV_H_

/*
 * decoupleYUYV.h
 *
 *  Created on: Aug 3, 2012
 *      Author: alexander
 */


#include "imageCaptureInterface.h"
#include "readers.h"
#include "fixedVector.h"
#include "sseWrapper.h"

namespace corecvs
{

class DecoupleYUYV
{
public:
    enum ImageCouplingType {
        ANAGLYPH_RC       = 0,
        SIDEBYSIDE_STEREO = 1,
        ANAGLYPH_RC_X2    = 2,
        ANAGLYPH_RC_FAST  = 3
    };


    static void decouple(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCouplingType coupling, ImageCaptureInterface::FramePair &result);
};

} /* namespace corecvs */
#endif /* DECOUPLE_YUYV_H_ */
