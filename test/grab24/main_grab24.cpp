#include <stdio.h>

#include <QApplication>
#include <QtCore/QCoreApplication>
#include <QtCore/QThread>
#include <unistd.h>


#include "core/utils/global.h"
#include "V4L2Capture.h"
#include "core/fileformats/bmpLoader.h"
#include "g12Image.h"

#include "advancedImageWidget.h"

#include "main_grab24.h"


int main (int argc, char **argv)
{

    QApplication app(argc, argv);
    Q_INIT_RESOURCE(main);

    SYNC_PRINT(("Attempting a grab\n"));

    V4L2CaptureInterface *input =
            static_cast<V4L2CaptureInterface*>(ImageCaptureInterface::fabric("v4l2:/dev/video0:1/10"));

    ImageCaptureInterface::CapErrorCode returnCode = input->initCapture();
    if (returnCode == ImageCaptureInterface::FAILURE)
    {
        SYNC_PRINT(("Can't open\n"));
        return 1;
    }

    FrameProcessor processor;
    processor.input = input;
    QObject::connect(
        input, SIGNAL(newFrameReady(frame_data_t)),
        &processor, SLOT(processFrame(frame_data_t)) /*,Qt::QueuedConnection*/);

	input->startCapture();

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

    //BMPLoader().save("snapshot.bmp", result);
    //delete_safe(result);
    delete_safe(input);
	return 0;
}

FrameProcessor::FrameProcessor(QObject *parent) :
    QObject(parent)
{

}

void FrameProcessor::processFrame(frame_data_t frameData)
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
    delete_safe(pair.bufferRight());
    delete_safe(pair.bufferLeft());
    delete_safe(pair.rgbBufferRight());

//    BMPLoader().save("cap.bmp", result);
    widget->setImage(QSharedPointer<QImage>(new RGB24Image(result)));
    delete_safe(result);
}

