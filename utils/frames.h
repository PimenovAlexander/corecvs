#pragma once

/**
 * \file frames.h
 * \brief Add Comment Here
 *
 * \date Mar 24, 2010
 * \author alexander
 */
#include <stdint.h>

#include "core/utils/global.h"

#include "core/buffers/g12Buffer.h"
#include "core/framesources/imageCaptureInterface.h"

class Frames
{
public:
    enum FrameSourceId {
        LEFT_FRAME        = ImageCaptureInterface::LEFT_FRAME,
        DEFAULT_FRAME     = LEFT_FRAME,
        RIGHT_FRAME       = ImageCaptureInterface::RIGHT_FRAME,
        MAX_INPUTS_NUMBER = ImageCaptureInterface::MAX_INPUTS_NUMBER
    };

    G12Buffer   *currentFrames   [MAX_INPUTS_NUMBER];
    RGB24Buffer *currentRgbFrames[MAX_INPUTS_NUMBER];
    int          frameCount;

    Frames();
   ~Frames();

    void        fetchNewFrames(ImageCaptureInterface *input);

    G12Buffer*   getCurrentFrame    (FrameSourceId id) { return currentFrames   [id]; }
    RGB24Buffer* getCurrentRgbFrame (FrameSourceId id) { return currentRgbFrames[id]; }

    /// Swaps the frame sources so, for example, left camera becomes right,
    /// and right camera becomes left. Useful when cameras are plugged in wrong order.
    void        swapFrameSources(bool shouldSwap);

    uint64_t    timestamp() const                      { return mTimestamp; }

    int64_t     desyncTime() const                     { return mDesyncTime; }

    uint64_t    startProcessTimestamp() const          { return mStartProcessTimestamp; }

    static inline const char *getEnumName(const FrameSourceId &value)
    {
        switch (value)
        {
            case LEFT_FRAME :   return "LEFT_FRAME";
            case RIGHT_FRAME:   return "RIGHT_FRAME";
            default:            break;
        }
        return "Not in range";
    }

private:
    bool     mSwapped;
    uint64_t mTimestamp;
    int64_t  mDesyncTime;
    uint64_t mStartProcessTimestamp;

    /* Not allowed to call*/
    //Frames(const Frames &);
};

/* EOF */
