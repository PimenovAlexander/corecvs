#include <stdio.h>
#include <core/math/vector/vector3d.h>

#ifdef WITH_LIBJPEG
#include "libjpegFileReader.h"
#endif

#ifdef WITH_LIBPNG
#include "libpngFileReader.h"
#endif

using namespace corecvs;

int main (int argC, char *argV[])
{
#ifdef WITH_LIBJPEG
    LibjpegFileReader::registerMyself();
    SYNC_PRINT(("Libjpeg support on\n"));
#endif
#ifdef WITH_LIBPNG
    LibpngFileReader::registerMyself();
    SYNC_PRINT(("Libpng support on\n"));
#endif

    printf("Autonav starts");
    Vector3dd a(1,2,3);
    cout << a << std::endl;

    return 0;
}
