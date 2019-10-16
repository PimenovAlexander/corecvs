#include <stdio.h>
#include <unistd.h>

#include "frameprocessor.h"
#include "opticalflowanalysis.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QThread>

#include "imageCaptureInterfaceQt.h"

#include "core/reflection/commandLineSetter.h"
#include "KLTFlow.h"




int main (int argc, char **argv)
{
    QApplication app(argc, argv);
    Q_INIT_RESOURCE(main);

    CommandLineSetter s(argc, argv);
//    std::string inputString = s.getString("input", "v4l2:/dev/video0:1/10");
    std::string inputString = "v4l2:/dev/video0:1/10";


    SYNC_PRINT(("Attempting a grab from \n"));
    ImageCaptureInterfaceQt *rawInput = ImageCaptureInterfaceQtFactory::fabric(inputString, true);

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

    OpticalFlowAnalysis analysis;

    Processor6D *proc = new OpenCVFlowProcessor();
    processor.optucalFlowProcessor = proc;

    analysis.opticalFlowProcessor = proc;

    QObject::connect(
        rawInput  , SIGNAL(newFrameReady(ImageCaptureInterface::FrameMetadata)),
        &processor,   SLOT(processFrame ()), Qt::QueuedConnection);

    QObject::connect(&processor, SIGNAL(flowCalculated()),
                     &analysis, SLOT(analyzeFlow()));

    rawInput->startCapture();

    AdvancedImageWidget widget;
    widget.show();
    processor.widget = &widget;



    app.exec();

    delete_safe(rawInput);
    return 0;
}
