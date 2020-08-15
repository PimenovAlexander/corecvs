#include <QApplication>
#include <patterndetection/dummyPatternDetector.h>
#include <patterndetection/harrisPatternDetector.h>
#include "qtFileLoader.h"

#include "core/utils/utils.h"
#include "core/geometry/mesh/mesh3d.h"
#include "core/geometry/mesh/mesh3DDecorated.h"
#include "core/reflection/commandLineSetter.h"
#include "core/buffers/bufferFactory.h"
#include "core/stereointerface/dummyFlowProcessor.h"

#include "server/server.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif
#ifdef WITH_OPENCV
#include "KLTFlow.h"
#include "patternDetect/openCVSquareDetector.h"
#include "patternDetect/openCVCheckerBoardDetector.h"
#endif
#ifdef WITH_APRILTAG
#include "wrappers/apriltag_wrapper/apriltagDetector.h"
#endif


#include "physicsMainWindow.h"

#define LIBEVENT_WSERVER

#ifdef LIBEVENT_WSERVER
#include "server/server.h"
#endif

using namespace corecvs;
using namespace std;

int mainExample1()
{/*
    Affine3DQ copterPos = Affine3DQ::Shift(10,10,10);

    Mesh3DDecorated *mesh = new Mesh3DDecorated;
    mesh->switchColor();
    mesh->mulTransform(copterPos);

    mesh->setColor(RGBColor::Green());
    mesh->addIcoSphere(Vector3dd( 5, 5, 0), 2, 2);
    mesh->addIcoSphere(Vector3dd(-5, 5, 0), 2, 2);

    mesh->setColor(RGBColor::Red());
    mesh->addIcoSphere(Vector3dd( 5, -5, 0), 2, 2);
    mesh->addIcoSphere(Vector3dd(-5, -5, 0), 2, 2);
    mesh->popTransform();

    mesh->dumpPLY("out.ply");
    printf("Hello\n");

*/

    return 0;
}


int main(int argc, char *argv[])
{
    SET_HANDLERS();
    Q_INIT_RESOURCE(main);

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
//    LibpngLoader<RuntimeTypeBuffer, true>::registerMyself(); //allow loading RGB24 as grayscale
    LibpngRuntimeTypeBufferLoader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif
    QTRGB24Loader::registerMyself();

    Processor6DFactoryHolder  ::getInstance()->registerProcessor(new AlgoFactory<DummyFlowProcessor, Processor6D>  ("Dummy"));
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<DummyFlowProcessor, ProcessorFlow>("Dummy"));

#ifdef WITH_OPENCV
    Processor6DFactoryHolder  ::getInstance()->registerProcessor(new AlgoFactory<OpenCVFlowProcessor, Processor6D  >("OpenCVProcessor"));
    ProcessorFlowFactoryHolder::getInstance()->registerProcessor(new AlgoFactory<OpenCVFlowProcessor, ProcessorFlow>("OpenCVProcessor"));
#endif

    PatternDetectorFabric::getInstance()->add(new PatternDetectorProducer<DummyPatternDetector>("Dummy"));
    PatternDetectorFabric::getInstance()->add(new PatternDetectorProducer<HarrisPatternDetector>("Harris"));

#ifdef WITH_OPENCV
    PatternDetectorFabric::getInstance()->add(new PatternDetectorProducer<OpenCVSquareDetector>("OpenCVSquare"));
    PatternDetectorFabric::getInstance()->add(new PatternDetectorProducer<OpenCVCheckerBoardDetector>("OpenCVCheckerBoard"));
#endif

#ifdef WITH_APRILTAG
    PatternDetectorFabric::getInstance()->add(new PatternDetectorProducer<ApriltagDetector>("Apriltag"));
#endif



    CommandLineSetter s(argc, argv);
    if (s.hasOption("caps"))
    {
        BufferFactory::printCaps();
        return 0;
    }

    SYNC_PRINT(("Starting Physics...\n"));
    QApplication app(argc, argv);


    PhysicsMainWindow *mainWindow = new PhysicsMainWindow;
    mainWindow->show();
    mainWindow->setAttribute(Qt::WA_QuitOnClose, true);
    mainWindow->setAttribute(Qt::WA_DeleteOnClose, true);

#ifdef LIBEVENT_WSERVER
    SYNC_PRINT(("Starting web server...\n"));
    startWServer();
    QObject::connect(mainWindow, SIGNAL(callingWServer()), server, SLOT(process_requests()));
#endif

    app.setQuitOnLastWindowClosed(true);
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    QObject::connect(mainWindow, SIGNAL(destroyed(QObject*)), &app, SLOT(quit()));
    app.exec();

    SYNC_PRINT(("Exiting\n"));
    return 0;
}
