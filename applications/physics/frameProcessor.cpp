#include "frameProcessor.h"

#include <QApplication>
#include <imageCaptureInterfaceQt.h>

#include "physicsMainWindow.h"

void FrameProcessor::processFrame(ImageCaptureInterface::FrameMetadata frameData)
{
    static int count=0;
    count++;

    static bool skipping = false;
    if (skipping)
    {
        return;
    }
    skipping = true;
    QApplication::processEvents();
    skipping = false;



//    SYNC_PRINT(("New frame arrived\n"));
    ImageCaptureInterface::FramePair pair = input->getFrameRGB24();
    RGB24Buffer * result = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

    target->uiMutex.lock();
    target->uiQueue.emplace_back(new DrawRequestData);
    target->uiQueue.back()->mImage = result;
    target->uiMutex.unlock();

    target->updateUi();

}
