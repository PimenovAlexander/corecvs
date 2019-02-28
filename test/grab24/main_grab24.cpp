#include <stdio.h>
#include <unistd.h>
#include <core/reflection/commandLineSetter.h>
#include <core/buffers/bufferFactory.h>


#include "core/utils/global.h"
#include "core/fileformats/bmpLoader.h"
#include "V4L2Capture.h"

#ifdef WITH_AVCODEC
#include "aviCapture.h"
#endif

class GrabReceiver : public ImageInterfaceReceiver
{
public:
    std::timed_mutex waitLock;

    virtual void newFrameReadyCallback(frame_data_t frameData) override
    {
       waitLock.unlock();
    }

    virtual void newImageReadyCallback() override {}
    virtual void newStatisticsReadyCallback(CaptureStatistics stats) override {}
    virtual void streamPausedCallback() override {}
    virtual void newCameraParamValueCallBack(int idParam) override {}

};

int main (int argc, char **argv)
{
    CommandLineSetter s(argc, argv);
    ImageCaptureInterfaceFabric::getInstance()->addProducer(new V4L2CaptureInterfaceProducer());
#ifdef WITH_AVCODEC
    ImageCaptureInterfaceFabric::getInstance()->addProducer(new AVICaptureProducer());
#endif

    ImageCaptureInterfaceFabric::printCaps();

    std::string inputString = s.getString("input", "v4l2:/dev/video0:1/10");

    SYNC_PRINT(("Attempting a grab from \n"));

    ImageCaptureInterface *rawInput = ImageCaptureInterfaceFabric::getInstance()->fabricate("v4l2:/dev/video0:1/10", true);
    V4L2CaptureInterface *input = dynamic_cast<V4L2CaptureInterface*>(rawInput);
    if (rawInput == NULL)
    {
        SYNC_PRINT(("Unable to fabricate camera grabber\n"));
        return -1;
    }

    SYNC_PRINT(("main: initialising capture...\n"));
    ImageCaptureInterface::CapErrorCode returnCode = rawInput->initCapture();
    SYNC_PRINT(("main: initialising capture returing %d\n", returnCode));

    if (returnCode == ImageCaptureInterface::FAILURE)
    {
        SYNC_PRINT(("Can't open\n"));
        return 1;
    }

    GrabReceiver reciever;
    input->imageInterfaceReceiver =  &reciever;
    reciever.waitLock.lock();

    input->startCapture();

    while (true)
    {

        if (reciever.waitLock.try_lock_for(std::chrono::seconds(10)))
        {
            SYNC_PRINT(("New frame arrived\n"));

            V4L2CaptureInterface::FramePair pair = input->getFrameRGB24();
            RGB24Buffer * result = pair.rgbBufferLeft();
            pair.setRgbBufferLeft(NULL);
            pair.freeBuffers();

            BufferFactory::getInstance()->saveRGB24Bitmap(result, "captured.bmp");
            delete_safe(result);
        } else {
            SYNC_PRINT(("I'm not Hachikō to wait for so long... something is broken\n"));
        }


        /* If you want you can continue to process next frames */
        break;

    }
    delete_safe(input);
	return 0;
}


