#include <stdio.h>
#include "capdll.h"
//#include <QApplication>
//#include <QThread>

#ifdef WIN32
# include <windows.h>
# define tSleep  Sleep
#else
# include <stdlib.h>
# define tSleep _sleep
#endif

void FrameCallbackFunctorExample(void *, DSCapDeviceId device, FrameData data)
{
    printf ("Here we are with %ld bytes for device %d\n", data.size, device);
}

#define PRINT_SIZE(X)  printf("sizeof(" #X ") = %d\n", (int)sizeof(X));

void greetingPrint( void )
{
    printf("Type   Size\n");
    PRINT_SIZE(long);
    PRINT_SIZE(bool);
    PRINT_SIZE(long long);
    PRINT_SIZE(int);
    PRINT_SIZE(void *);
    PRINT_SIZE(CAPTURE_FORMAT_TYPE);
    PRINT_SIZE(CameraParameter);
    PRINT_SIZE(CAPDLLControlProperty);
    PRINT_SIZE(CaptureTypeFormat);
    PRINT_SIZE(FrameData);
    fflush(stdout);
}

int main (int argc, char * argv[])
{
    greetingPrint();
//        setlocale(LC_ALL,".1251");
//        wprintf(L"Это <%ls>\n", L"Тест"); fflush(stdout);
//    QApplication app(argc, argv);

    int camId = 1;
    if (argc > 1 && strlen(argv[1]) == 1) {
        camId = atoi(argv[1]);
    }

    int numCams;
    DirectShowCapDll_devicesNumber(&numCams);
    printf ("Found %d devices that support DirectShow interface.\n", numCams);

    if (camId >= numCams) {
        printf ("Invalid camId=%d, there're found %d device(s) that support DirectShow interface.\n", camId, numCams);
        camId = 0;
    }

    DSCapDeviceId cam = DirectShowCapDll_initCapture(camId);
    DirectShowCapDll_setFrameCallback(cam, NULL, FrameCallbackFunctorExample);
    DirectShowCapDll_start(cam);
    tSleep(1000);  // = 1s
    DirectShowCapDll_stop(cam);
}
