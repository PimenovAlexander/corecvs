#include <fstream>

#include "core/reflection/commandLineSetter.h"
#include "core/fileformats/meshLoader.h"
#include "core/fileformats/plyLoader.h"
#include "core/utils/utils.h"


#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#include "server.h"

using namespace corecvs;
using namespace std;

int main (int argc, char **argv)
{
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    LibpngFileSaver::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif


    CommandLineSetter s(argc, argv);

    SYNC_PRINT(("Starting web server...\n"));
    startWServer();

    while (true) {
        usleep(10);
        server->process_requests();
    }
    SYNC_PRINT(("Exiting web server...\n"));
    return 0;
}

