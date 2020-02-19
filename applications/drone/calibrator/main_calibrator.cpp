#include <QApplication>
#include "fileformats/qtFileLoader.h"

#include "utils/utils.h"
#include "geometry/mesh/mesh3d.h"
#include "geometry/mesh/mesh3DDecorated.h"
#include "reflection/commandLineSetter.h"
#include "buffers/bufferFactory.h"
#include "stereointerface/dummyFlowProcessor.h"

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif
#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif
#ifdef WITH_OPENCV
#include "KLTFlow.h"
#include "patternDetect/openCVSquareDetector.h"
#endif
#ifdef WITH_APRILTAG
#include "apriltagDetector.h"
#endif

#include "physicsMainWindow.h"

using namespace corecvs;
using namespace std;

int main(int argc, char *argv[])
{
    SET_HANDLERS();
    Q_INIT_RESOURCE(main);

    QTRGB24Loader::registerMyself();
    QTRGB24Saver::registerMyself();

    BufferFactory::printCaps();

    CommandLineSetter s(argc, argv);
    if (s.hasOption("caps"))
    {
        BufferFactory::printCaps();
        return 0;
    }

    SYNC_PRINT(("Starting Calibrator...\n"));
    ImageCaptureInterfaceFabric::printCaps();
    QApplication app(argc, argv);


    CalibrationWidget mainWindow;
    mainWindow.show();
    mainWindow.setAttribute(Qt::WA_QuitOnClose, true);

    app.setQuitOnLastWindowClosed(true);
    QObject::connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    app.exec();

    SYNC_PRINT(("Exiting...\n"));
    return 0;
}
