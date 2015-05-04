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
        ANAGLYPH_RC       , /**< Decodes two stereo images form Red/Cyan anaglyth */
        SIDEBYSIDE_STEREO , /**< Decodes two stereo images form side by side buffer */
        ANAGLYPH_RC_X2    , /**< Decodes two stereo images form Red/Cyan anaglyth with unrolled inner cylce (for profiling only)*/
        ANAGLYPH_RC_FAST  , /**< Decodes two stereo images form Red/Cyan anaglyth with optional SSE optimisation */

        SIDEBYSIDE_SYNCCAM_1  /**< First version SyncCam Linux version */
    };


    static void decouple                (unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCouplingType coupling, ImageCaptureInterface::FramePair &result);

    static void decoupleSideBySide      (unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result);
    static void decoupleAnaglythUnrolled(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result);
    static void decoupleAnaglythSSE     (unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result, bool withSSE = true);

    static void decoupleAnaglythSyncCam1(unsigned formatH, unsigned formatW, uint8_t *ptr, ImageCaptureInterface::FramePair &result);

};

} /* namespace corecvs */
#endif /* DECOUPLE_YUYV_H_ */
