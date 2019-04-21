#include "frameprocessor.h"

#include "core/utils/global.h"
#include "core/fileformats/bmpLoader.h"
#include "V4L2Capture.h"
#include "g12Image.h"
#include "KLTFlow.h"

#include <core/buffers/bufferFactory.h>



using namespace corecvs;

FrameProcessor::FrameProcessor(QObject *parent) : QObject(parent)
{

}

void FrameProcessor::processFrame()
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

    V4L2CaptureInterface::FramePair pair = input->getFrameRGB24();
    RGB24Buffer *cFrame = pair.rgbBufferLeft();
    pair.setRgbBufferLeft(NULL);
    pair.freeBuffers();

    this->optucalFlowProcessor->beginFrame();
    this->optucalFlowProcessor->setFrameRGB24(Processor6D::FRAME_LEFT_ID, cFrame);
    this->optucalFlowProcessor->endFrame();
    emit flowCalculated();

    RGB24Buffer result(cFrame);

    FlowBuffer *flow = this->optucalFlowProcessor->getFlow();
    if (flow != NULL) {
        result.drawFlowBuffer3(flow);
    }

    widget->setImage(QSharedPointer<QImage>(new RGB24Image(&result)));

    delete_safe(cFrame);
}
