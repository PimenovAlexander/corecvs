#include <QApplication>
#include "qtFileLoader.h"

#include "core/utils/utils.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/mesh3DDecorated.h"
#include "core/reflection/commandLineSetter.h"
#include "core/buffers/bufferFactory.h"
#include "core/stereointerface/dummyFlowProcessor.h"

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
#include "wrappers/apriltag_wrapper/apriltagDetector.h"
#endif


#include "physicsMainWindow.h"

using namespace corecvs;
using namespace std;

int main(int argc, char *argv[])
{
    SET_HANDLERS();
    Q_INIT_RESOURCE(main);

    QTRGB24Loader::registerMyself();

    CommandLineSetter s(argc, argv);
    if (s.hasOption("caps"))
    {
        BufferFactory::printCaps();
        return 0;
    }

    SYNC_PRINT(("Starting Calibrator...\n"));
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
