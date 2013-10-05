#ifndef uEyeCameraDescriptor_H
#define uEyeCameraDescriptor_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include <uEye.h>

#include <QtGui/QImage>

#include "global.h"

struct sCameraProps
{
    bool usesImageFormats;
    int imgFmtNormal;
    int imgFmtTrigger;
};

struct sBufferProps
{
    int width;
    int height;
    int colorformat;
    int bitspp;
    QImage::Format imgformat;
    int imageformat;
};

class BufferDescriptorType
{
public:

    uint8_t *buffer;
    INT imageID;
    INT imageSeqNum;
    INT bufferSize;

    /* Some metainfo about particular frame */

    uint64_t internalTimestamp;
    UEYETIME pcTimestamp;
    int buffersInCamera;

    uint64_t usecsTimeStamp()
    {
       /* printf("Timestamp: %d, %d %d %d:%d:%d (%d ms) =>",
                pcTimestamp.wYear,
                pcTimestamp.wMonth,
                pcTimestamp.wDay,
                pcTimestamp.wHour,
                pcTimestamp.wMinute,
                pcTimestamp.wSecond,
                pcTimestamp.wMilliseconds);*/

        time_t rawtime;
        time ( &rawtime );

        struct tm *timeinfo = localtime ( &rawtime );

        /*printf("(Curr: %d, %d %d %d:%d:%d) =>",
               timeinfo->tm_year,
               timeinfo->tm_mon,
               timeinfo->tm_mday,
               timeinfo->tm_hour,
               timeinfo->tm_min,
               timeinfo->tm_sec);*/

        timeinfo->tm_year = pcTimestamp.wYear - 1900;
        timeinfo->tm_mon  = pcTimestamp.wMonth;
        timeinfo->tm_mday = pcTimestamp.wDay;
        timeinfo->tm_hour = pcTimestamp.wHour;
        timeinfo->tm_min  = pcTimestamp.wMinute;
        timeinfo->tm_sec  = pcTimestamp.wSecond;
        uint64_t secTimeStamp = mktime ( timeinfo );

   /*     printf("%" PRIu64 "\n", secTimeStamp);*/

        if (secTimeStamp == (uint64_t)(-1))
            return 0;
        return (secTimeStamp * 1000 + pcTimestamp.wMilliseconds) * 1000;
    }

    BufferDescriptorType()
    {
        memset(this, 0, sizeof(BufferDescriptorType));
    }
};

class UEyeCameraDescriptor
{
public:

    INT deviceID;
    HIDS mCamera;

#ifdef Q_OS_WIN
    HANDLE mWinEvent;
#endif

    static const unsigned IMAGE_BUFFER_COUNT = 5;

    BufferDescriptorType images[IMAGE_BUFFER_COUNT];

    struct sCameraProps cameraProps;
    struct sBufferProps bufferProps;

    int searchDefImageFormats(int suppportMask);
    static int getBitsPerPixel(int colormode);
    QImage::Format getQImageFormat(int colormode);

    int initBuffer();
    int init(int deviceID, bool binning, bool globalShutter = true, int pixelClock=85, double fps=50.0);
    int waitUEyeFrameEvent(int timeout);

    uint16_t* getFrame();

    bool allocImages();
    bool deAllocImages();

    BufferDescriptorType* getDescriptorByAddress (char* pbuf);

    double getTemperature();
};


#endif // UEyeCameraDescriptor_H
