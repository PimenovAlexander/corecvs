#include <iostream>

#include <reflection/commandLineSetter.h>
#include <reflection/jsonPrinter.h>
#include <reflection/usageVisitor.h>

#include <jsonModernReader.h>

#include <stereointerface/dummyFlowProcessor.h>

#include "stereointerface/processor6D.h"

#include "buffers/bufferFactory.h"
#include "fileformats/bmpLoader.h"
#include "buffers/rgb24/rgb24Buffer.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif
#ifdef WITH_OPENCV
#include <KLTFlow.h>
#include <PCAFlow/PCAFlowProcessor.h>
#endif
#ifdef WITH_DISFLOW
#include <DISFlow/DISFlow.h>
#endif
#ifdef WITH_AVCODEC
#include "aviCapture.h"
#endif


using namespace std;
using namespace corecvs;

int producerCaps(CommandLineSetter &s)
{
    std::string producerName = s.getString("producer");
    ProcessorFlow *producer = ProcessorFlowFactoryHolder::getInstance()->getProcessor(producerName);

    if (producer == NULL) {
        SYNC_PRINT(("Producer not found. Check --caps\n"));
        return 1;
    }

    SYNC_PRINT(("Printing parameters for <%s>\n", producerName.c_str()));

    std::map<std::string, DynamicObject> paramsMap = producer->getParameters();
    for(auto &it : paramsMap) {
        cout << it.first << endl;
        UsagePrinter printer;
        DynamicObjectWrapper wrapper(it.second.reflection, it.second.rawObject);
        printer.printUsage(&wrapper, it.first + ".");
        cout << endl;
    }
    SYNC_PRINT(("You can also use config files:\n"));
    SYNC_PRINT(("  --config=<filename> - this JSON file is loaded first, separate parameters are applied later\n"));
    SYNC_PRINT(("  --dumpConfig - saves the default file\n"));
    SYNC_PRINT(("\n"));
    return 0;
}


ProcessorFlow * getFlowProcessor(CommandLineSetter &s)
{
    std::string producerName = s.getString("producer");
    ProcessorFlow *producer = ProcessorFlowFactoryHolder::getInstance()->getProcessor(producerName);
    if (producer == NULL) {
        SYNC_PRINT(("Producer not found. Check --caps\n"));
        return NULL;
    }
    std::map<std::string, DynamicObject> paramsMap = producer->getParameters();
    std::string configFile = s.getString("config", "");

    /* Store default config if needed */
    if (s.hasOption("dumpConfig")) {
        if (configFile.empty()) {
            cout << "Config example begins below this line:" << endl;
            JSONPrinter printer;
            for(auto &it : paramsMap)
                printer.visit(it.second, it.first.c_str());
            cout << endl << "Config ended\n";
        } else {
            JSONPrinter printer(configFile);
            for(auto &it : paramsMap)
                printer.visit(it.second, it.first.c_str());
        }
        return 0;
    } else if (!configFile.empty()) {
        JSONModernReader reader(configFile);
        if (!reader.hasError()) {
            for(auto &it : paramsMap)
                reader.visit(it.second, it.first.c_str());
        }
    }

    /* Check for config parameters from command line */
    for(auto &it : paramsMap) {
        CommandLineSetter sCopy = s;
        sCopy.mPreserveValues = true;

        /*This block is unsafe*/
        DynamicObject copy = it.second.clone();
        sCopy.visit(it.second, it.first.c_str());
        if (!DynamicObject::compare(copy, it.second))
            cout << "Params Differ" << endl;
    }

    if (s.hasOption("showParams"))
        for(auto &it : paramsMap)
            cout << "Param set:\n" << it.first << endl << it.second << endl;

    /*Setting actual parameters*/
    for(auto &it : paramsMap)
        producer->setParameters(it.first, it.second);

    return producer;
}

int flow(CommandLineSetter &s)
{
    Statistics stats;

    Statistics::startInterval(&stats);
    ProcessorFlow *producer = getFlowProcessor(s);
    Statistics::endInterval(&stats, "Creating producer");
    if (producer == NULL) {
        return 2;
    }

    Statistics::startInterval(&stats);
    std::string input1Name = s.getString("input1");
    RGB24Buffer *input1 = BufferFactory::getInstance()->loadRGB24Bitmap(input1Name);
    if (input1 == NULL)
    {
        SYNC_PRINT(("Unable to load image1 <%s>. Add --input1=<filename> parameter.\n", input1Name.c_str()));
        return 1;
    }

    std::string input2Name = s.getString("input2");
    RGB24Buffer *input2 = BufferFactory::getInstance()->loadRGB24Bitmap(input2Name);
    if (input2 == NULL)
    {
        SYNC_PRINT(("Unable to load image2 <%s>. Add --input2=<filename> parameter.\n", input2Name.c_str()));
        return 1;
    }

    std::string input3Name = s.getString("input3");
    RGB24Buffer *input3 = BufferFactory::getInstance()->loadRGB24Bitmap(input3Name);
    if (input3 == NULL)
    {
        SYNC_PRINT(("Unable to load image2 <%s>. Add --input3=<filename> parameter.\n", input3Name.c_str()));
    }

    SYNC_PRINT(("Images : 1 [%d x %d]  2 [%d x %d]  ", input1->w, input1->h, input2->w, input2->h));
    if (input3) {
        SYNC_PRINT(("3 [%d x %d]", input3->w, input3->h));
    }
    SYNC_PRINT(("\n"));

    Statistics::endInterval(&stats, "Loading input data");

    Statistics::enterContext(&stats, "Processing ->");
    producer->requestResultsi(ProcessorFlow::RESULT_FLOW);
    producer->setStats(&stats);
    producer->beginFrame();
    producer->setFrameRGB24(input1);
    producer->endFrame();

    producer->beginFrame();
    producer->setFrameRGB24(input2);
    producer->endFrame();

    FlowBuffer *flow = producer->getFlow();
    cout << "Density:" << flow->density() << endl;
    FloatFlowBuffer *fflow = new FloatFlowBuffer(flow);
    BufferFactory::getInstance()->saveFloatFlow(*fflow, "flow1.flo");

    {
        RGB24Buffer flowDraw(fflow->getSize());
        flowDraw.drawFlowBuffer(fflow, -1, 0, 0);
        BufferFactory::getInstance()->saveRGB24Bitmap(flowDraw, "flow.bmp");
    }

    delete_safe(fflow);

    if (input3) {
        producer->beginFrame();
        producer->setFrameRGB24(input2);
        producer->endFrame();

        FlowBuffer *flow2 = producer->getFlow();
        cout << "Density2:" << flow2->density() << endl;
    }

    Statistics::leaveContext(&stats);
    Statistics::startInterval(&stats);
   // producer->dumpAllDebugs("detector-", ".png");

    delete_safe(input1);
    delete_safe(input2);

    delete_safe(producer);
    Statistics::endInterval(&stats, "Cleanup");

    BaseTimeStatisticsCollector collector;
    collector.addStatistics(stats);
    collector.printAdvanced();

    return 0;
}

#ifdef WITH_AVCODEC
int flowVideo(CommandLineSetter &s)
{
    Statistics stats;

    Statistics::startInterval(&stats);
    ProcessorFlow *producer = getFlowProcessor(s);
    Statistics::endInterval(&stats, "Creating producer");
    if (producer == NULL) {
        return 2;
    }
    std::string input = s.getString("input");
    AviCapture *capture = new AviCapture(input);

    ImageCaptureInterface::CapErrorCode res = capture->initCapture();
    cout << res << endl;
    if (res == ImageCaptureInterface::FAILURE) {
        return 3;
    }

    capture->startCapture();

    producer->requestResultsi(ProcessorFlow::RESULT_FLOW | ProcessorFlow::RESULT_FLOAT_FLOW);
    producer->setStats(&stats);

    int firstframe = s.getInt("start", 0);
    int lastframe = s.getInt("frames", numeric_limits<int>::max());

    SYNC_PRINT(("Skipping %d frames:\n", firstframe));
    for (int i = 0; i < firstframe; i++)
    {
        ImageCaptureInterface::FramePair pair = capture->getFrameRGB24();
        pair.freeBuffers();
    }

    SYNC_PRINT(("Would process %d frames:\n", lastframe));
    for (int i = 0; i < lastframe; i++)
    {
        ImageCaptureInterface::FramePair pair = capture->getFrameRGB24();
        RGB24Buffer * frame = pair.rgbBufferLeft();
        pair.setRgbBufferLeft(NULL);
        pair.freeBuffers();
        SYNC_PRINT(("New frame...\n"));

        if (frame == NULL)
            return 5;

        producer->beginFrame();
        producer->setFrameRGB24(frame);
        producer->endFrame();

#if 0
        FloatFlowBuffer *flow = producer->getFloatFlow();

        if (flow != NULL)
        {
            std::ostringstream oss;
            oss << "flow" << (i + firstframe) << ".png";

            RGB24Buffer flowDraw(frame);
            flowDraw.drawFlowBuffer(flow);
            BufferFactory::getInstance()->saveRGB24Bitmap(flowDraw, oss.str());
        } else {
            SYNC_PRINT(("Flow is NULL\n"));
        }
#else
        FlowBuffer *flow = producer->getFlow();

        if (flow != NULL)
        {
            std::ostringstream oss;
            oss << "flow" << (i + firstframe) << ".png";

            RGB24Buffer flowDraw(frame);
            flowDraw.drawFlowBuffer1(flow);
            BufferFactory::getInstance()->saveRGB24Bitmap(flowDraw, oss.str());
        } else {
            SYNC_PRINT(("Flow is NULL\n"));
        }
#endif

        delete_safe(frame);
    }

    BaseTimeStatisticsCollector collector;
    collector.addStatistics(stats);
    collector.printAdvanced();

    return 0;
}
#endif


void usage()
{
  SYNC_PRINT(("Call example:\n\n"));

  SYNC_PRINT(("./bin/flow_detector --caps\n"));
  SYNC_PRINT(("          - show options with which app was compiled\n"));
  SYNC_PRINT(("./bin/flow_detector --producercaps --producer=Dummy\n"));
  SYNC_PRINT(("          - show specific provider options\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=Dummy --dumpConfig\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=OpenCVProcessor --dumpConfig\n"));
  SYNC_PRINT(("          - dump current config to stdout\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=Dummy --dumpConfig --config=out.json\n"));
  SYNC_PRINT(("          - dump current config to out.json\n"));
  SYNC_PRINT(("          \n"));
  SYNC_PRINT(("          \n"));
  SYNC_PRINT(("Dummy pattern provider:\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=Dummy --input1=000_001800.png --input2=000_001801.png\n"));
  SYNC_PRINT(("          - example that returns dummy data\n"));
  SYNC_PRINT(("          \n"));
  SYNC_PRINT(("KLT flow provider:\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=OpenCVProcessor --params.debug=on --input=photo_2019-09-29_23-11-36.jpg\n"));
  SYNC_PRINT(("          \n"));
  SYNC_PRINT(("PCA flow provider:\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=OpenCVPCAFlow --params.debug=on --input=photo_2019-09-29_23-11-36.jpg\n"));
  SYNC_PRINT(("          \n"));
  SYNC_PRINT(("DIS flow provider:\n"));
  SYNC_PRINT(("./bin/flow_detector --flow --producer=DISFlow --params.debug=on --input=photo_2019-09-29_23-11-36.jpg\n"));
  SYNC_PRINT(("          \n"));

#ifdef WITH_AVCODEC
  SYNC_PRINT(("./bin/flow_detector --video --producer=Dummy --input=../05032102_0005-5-of-11.MP4 --frames=20\n"));
#endif


}

int main(int argc, char *argv[])
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    LibjpegFileSaver::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif    
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<DummyFlowProcessor, ProcessorFlow>("Dummy"));
#ifdef WITH_OPENCV
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<OpenCVFlowProcessor, ProcessorFlow>("OpenCVProcessor"));
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<PCAFlowProcessor, ProcessorFlow>("OpenCVPCAFlow"));
#ifdef WITH_DISFLOW
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<DISFlow, ProcessorFlow>("DISFlow"));
#endif
#endif

    CommandLineSetter s(argc, argv);
    if (s.hasOption("caps")) {
        BufferFactory::printCaps();
        ProcessorFlowFactoryHolder::getInstance()->printCaps();
        return 0;
    }

    if (s.hasOption("producercaps")) {
        producerCaps(s);
        return 0;
    }

    if (s.hasOption("flow"))
    {
        flow(s);
        return 0;
    }

#if WITH_AVCODEC
    if (s.hasOption("video"))
    {
        flowVideo(s);
        return 0;
    }
#endif

    usage();
    return 0;
}
