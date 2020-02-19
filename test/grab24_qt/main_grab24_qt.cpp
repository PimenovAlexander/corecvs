#include <stdio.h>
#include <unistd.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QThread>


#include "main_grab24_qt.h"

#include "core/reflection/commandLineSetter.h"
#include "core/utils/global.h"
#include "core/fileformats/bmpLoader.h"
#include "V4L2Capture.h"
#include "corestructs/g12Image.h"
#include "framesources/imageCaptureInterfaceQt.h"
#include "uis/advancedImageWidget.h"


int main (int argc, char **argv)
{

    QApplication app(argc, argv);
    Q_INIT_RESOURCE(main);
    CommandLineSetter s(argc, argv);

    /*if (s.hasOption("caps"))
    {
        ImageCaptureInterfaceQtFactory::
        return 0;
    }*/

    std::string inputString = s.getString("input", "v4l2:/dev/video0:1/10");


    SYNC_PRINT(("Attempting a grab from \n"));
/*
    ImageCaptureInterface *rawInput = ImageCaptureInterfaceFabric::getInstance()->fabricate("v4l2:/dev/video0:1/10", true);
    V4L2CaptureInterface *input = dynamic_cast<V4L2CaptureInterface*>(rawInput);
*/

    ImageCaptureInterfaceQt *rawInput = ImageCaptureInterfaceQtFactory::fabric(inputString, true);
    if (rawInput == NULL)
    {
        SYNC_PRINT(("Unable to fabricate camera grabber\n"));
    }

    //ImageCaptureInterfaceWrapper<V4L2CaptureInterface> *input = dynamic_cast<ImageCaptureInterfaceWrapper<V4L2CaptureInterface>*>(rawInput);

    SYNC_PRINT(("main: initialising capture...\n"));
    ImageCaptureInterface::CapErrorCode returnCode = rawInput->initCapture();
    SYNC_PRINT(("main: initialising capture returing %d\n", returnCode));

    if (returnCode == ImageCaptureInterface::FAILURE)
    {
        SYNC_PRINT(("Can't open\n"));
        return 1;
    }

    FrameProcessor processor;
    processor.input = rawInput;
    QObject::connect(
        rawInput  , SIGNAL(newFrameReady(ImageCaptureInterface::FrameMetadata)),
        &processor,   SLOT(processFrame (ImageCaptureInterface::FrameMetadata)), Qt::QueuedConnection);

    rawInput->startCapture();

#if 0
	RGB24Buffer *result = NULL;

	for(int i = 0; i < 40; i++) {
	    delete_safe(result);
	    V4L2CaptureInterface::FramePair pair = input->getFrameRGB24();
	    result = pair.rgbBufferLeft;
	    delete_safe(pair.bufferRight);
	    delete_safe(pair.bufferLeft);
        delete_safe(pair.rgbBufferRight);
	}
#endif

    AdvancedImageWidget widget;
    widget.show();
    processor.widget = &widget;



    app.exec();

    delete_safe(rawInput);
	return 0;
}

FrameProcessor::FrameProcessor(QObject *parent) :
    QObject(parent)
{

}


void FrameProcessor::processFrame(ImageCaptureInterface::FrameMetadata /*frameData*/)
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
    V4L2CaptureInterface::FramePair pair = input->getFrameRGB24();
    RGB24Buffer * result = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

//    BMPLoader().save("cap.bmp", result);
    if (result != NULL) {
        widget->setImage(QSharedPointer<QImage>(new RGB24Image(result)));
    }
    delete_safe(result);
}

