#ifndef uEyeCameraDescriptor_H
#define uEyeCameraDescriptor_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <ostream>

#include <uEye.h>

#include <QImage>

#include "core/utils/global.h"

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

    friend std::ostream& operator << (std::ostream &out, sBufferProps &toSave)
    {
        out << "Buffer" << "[" << toSave.width << " x " << toSave.height << "]" << std::endl;
        out << "  Format " << toSave.colorformat << std::endl;
        out << "  Bits Per Pixel " << toSave.bitspp << std::endl;
        return out;
    }
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
    bool inited;
    INT deviceID;
    HIDS mCamera;

#ifdef Q_OS_WIN
    HANDLE mWinEvent;
#endif

    UEyeCameraDescriptor() :
        inited(false),
        deviceID(-1),
        mCamera(0)
    {}

    static const unsigned IMAGE_BUFFER_COUNT = 5;
    BufferDescriptorType images[IMAGE_BUFFER_COUNT];

    struct sCameraProps cameraProps;
    struct sBufferProps bufferProps;

    int searchDefImageFormats(int suppportMask);

    int init(int deviceID, int binning, bool globalShutter = true, int pixelClock=85, double fps=50.0, bool isRgb = false);

    int initBuffer();   
    int waitUEyeFrameEvent(int timeout);

    uint16_t* getFrame();

    bool allocImages();
    bool deallocImages();

    BufferDescriptorType* getDescriptorByAddress (char* pbuf);

    double getTemperature();

    static int getBitsPerPixel(int colormode);
    static QImage::Format getQImageFormat(int colormode);
};


#endif // UEyeCameraDescriptor_H
