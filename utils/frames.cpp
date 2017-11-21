/**
 * \file Frames.cpp
 * \brief Add Comment Here
 *
 * \date Mar 24, 2010
 * \author alexander
 */

/*
#ifndef TRACE
#define TRACE
#endif
*/
#include <QtCore/QSettings>

#include "core/utils/global.h"

#include "frames.h"

Frames::Frames()
  : mSwapped(false)
  , mTimestamp(0)
  , mDesyncTime(0)
  , mStartProcessTimestamp(0)
{
    DOTRACE(("Frames::Frames() called\n"));
    for (int id = 0; id < MAX_INPUTS_NUMBER; id++)
    {
        currentFrames[id]    = NULL;
        currentRgbFrames[id] = NULL;
    }
    frameCount = 0;

    /* Remove this. BaseParametersWidget should store the state*/
    /*QSettings settings("Lanit-Tercom", "CVS");
    mSwapped = settings.value("swapCameras", false).toBool();*/
}

/**
 **/
Frames::~Frames()
{
    for (int id = 0; id < MAX_INPUTS_NUMBER; id++)
    {
        delete_safe(currentFrames[id]);
        delete_safe(currentRgbFrames[id]);
    }
}

void Frames::fetchNewFrames(ImageCaptureInterface *input)
{
//    SYNC_PRINT(("Frames::fetchNewFrames():Swapped is %d\n", mSwapped));

    ImageCaptureInterface::FramePair pair = input->mIsRgb ? input->getFrameRGB24() : input->getFrame();
    if (!pair.hasBoth())
    {
        SYNC_PRINT(("Alert: We have received one or zero frames\n"));
    }

    for (int id = 0; id < MAX_INPUTS_NUMBER; id++)
    {
        delete_safe(currentFrames[id]);
        delete_safe(currentRgbFrames[id]);
    }

    currentFrames[LEFT_FRAME]  = mSwapped ? pair.bufferRight() : pair.bufferLeft();
    currentFrames[RIGHT_FRAME] = mSwapped ? pair.bufferLeft()  : pair.bufferRight();

    if (input->mIsRgb)
    {
//        printf("Frames::fetchNewFrames() : rgb\n");
        currentRgbFrames[LEFT_FRAME]  = mSwapped ? pair.rgbBufferRight() : pair.rgbBufferLeft();
        currentRgbFrames[RIGHT_FRAME] = mSwapped ? pair.rgbBufferLeft()  : pair.rgbBufferRight();
    }
    else
    {
//        printf("Frames::fetchNewFrames() : no rgb\n");
        currentRgbFrames[LEFT_FRAME]  = NULL;
        currentRgbFrames[RIGHT_FRAME] = NULL;
    }

    for (int i = RIGHT_FRAME + 1; i < MAX_INPUTS_NUMBER; i++)
    {
         currentFrames   [i] = pair.buffers[i].g12Buffer;
         currentRgbFrames[i] = pair.buffers[i].rgbBuffer;
    }


    mTimestamp  = pair.timeStamp();

    mDesyncTime = !mSwapped ? pair.diffTimeStamps() : -pair.diffTimeStamps();
    frameCount++;


#if 0
    SYNC_PRINT(("Frames::fetchNewFrames(): G12: ["));
    for (int i = 0; i < MAX_INPUTS_NUMBER; i++)
    {
         SYNC_PRINT(("%s", currentFrames[i] == NULL ? "-" : "+"));
    }
    SYNC_PRINT(("]\n"));
    SYNC_PRINT(("Frames::fetchNewFrames(): RGB: %s [", input->mIsRgb ? "expected" : "off"));
    for (int i = 0; i < MAX_INPUTS_NUMBER; i++)
    {
         SYNC_PRINT(("%s", currentRgbFrames[i] == NULL ? "-" : "+"));
    }
    SYNC_PRINT(("]\n"));
#endif

}

void Frames::swapFrameSources(bool shouldSwap)
{
    mSwapped = shouldSwap;
    QSettings settings("cvs-camera.conf", QSettings::IniFormat);
    settings.setValue("swapCameras", mSwapped);
}
