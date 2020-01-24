#include <iostream>

#include "uEyeCameraDescriptor.h"
#include "ueye_deprecated.h"

int UEyeCameraDescriptor::getBitsPerPixel(int colormode)
{
    switch (colormode)
    {
        default:
        case IS_CM_MONO8:
        case IS_CM_BAYER_RG8:                // deprecated since uEye 4.20. subject to delete
//        case IS_CM_SENSOR_RAW8:
            return 8;   // occupies 8 Bit
        case IS_CM_MONO12:
        case IS_CM_MONO16:
        case IS_CM_BAYER_RG12:               // deprecated since uEye 4.20. subject to delete
        case IS_CM_BAYER_RG16:               // deprecated since uEye 4.20. subject to delete
//        case IS_CM_SENSOR_RAW12:
//        case IS_CM_SENSOR_RAW16:
        case IS_CM_BGR555_PACKED:            // deprecated since uEye 4.20. subject to delete
//        case IS_CM_BGR5_PACKED:
        case IS_CM_BGR565_PACKED:
        case IS_CM_UYVY_PACKED:
        case IS_CM_CBYCRY_PACKED:
            return 16;  // occupies 16 Bit
        case IS_CM_RGB8_PACKED:
        case IS_CM_BGR8_PACKED:
            return 24;
        case IS_CM_RGBA8_PACKED:
        case IS_CM_BGRA8_PACKED:
        case IS_CM_RGBY8_PACKED:
        case IS_CM_BGRY8_PACKED:
        case IS_CM_RGB10V2_PACKED:           // deprecated since uEye 4.20. subject to delete
        case IS_CM_BGR10V2_PACKED:           // deprecated since uEye 4.20. subject to delete
//        case IS_CM_BGR10_PACKED:
//        case IS_CM_RGB10_PACKED:
            return 32;
    }
}

QImage::Format UEyeCameraDescriptor::getQImageFormat(int colormode)
{
   switch (colormode)
   {
       case IS_CM_BGR565_PACKED:
           return QImage::Format_RGB16;
           break;
       case IS_CM_RGB8_PACKED:
       case IS_CM_BGR8_PACKED:
           return QImage::Format_RGB888;
           break;
       default:
       case IS_CM_RGBA8_PACKED:
       case IS_CM_BGRA8_PACKED:
           return QImage::Format_RGB32;
           break;
   }
   return QImage::Format_Invalid;
}

bool UEyeCameraDescriptor::allocImages()
{
    int width = 200;
    int height = 100;

    UINT absPosX;
    UINT absPosY;

    if (is_AOI(mCamera, IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&absPosX , sizeof(absPosX)) != IS_SUCCESS)
    {
        SYNC_PRINT(("UEyeCameraDescriptor::allocImages(): is_AOI failed for X\n"));
    }

    if (is_AOI(mCamera, IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&absPosY , sizeof(absPosY)) != IS_SUCCESS)
    {
        SYNC_PRINT(("UEyeCameraDescriptor::allocImages(): is_AOI failed for Y\n"));
    }

    if (is_ClearSequence(mCamera) != IS_SUCCESS)
    {
        SYNC_PRINT(("UEyeCameraDescriptor::allocImages(): is_ClearSequence failed\n"));
    }
    for (unsigned int i = 0; i < IMAGE_BUFFER_COUNT; i++)
    {
        if (images[i].buffer)
        {
            is_FreeImageMem (mCamera, (char *)images[i].buffer, images[i].imageID);
            images[i].buffer = NULL;
            images[i].imageID = 0;
        }

        width = bufferProps.width;
        height = bufferProps.height;

        if (absPosX)
        {
            bufferProps.width = width;
        }
        if (absPosY)
        {
            bufferProps.height = height;
        }

        if (is_AllocImageMem (
                mCamera,
                width, height,
                bufferProps.bitspp, (char **)&images[i].buffer, &images[i].imageID) != IS_SUCCESS)
        {
            SYNC_PRINT(("Unable to allocate buffers\n"));
            return false;
        }

        if (is_AddToSequence (mCamera, (char *)images[i].buffer, images[i].imageID) != IS_SUCCESS)
        {
            SYNC_PRINT(("Unable to append buffers to sequence\n"));
            return false;
        }

        images[i].imageSeqNum = i + 1;
        images[i].bufferSize = (width * height * bufferProps.bitspp) / 8;
    }

    return TRUE;
}

bool UEyeCameraDescriptor::deallocImages()
{
    for (unsigned int i = 0; i < sizeof(images) / sizeof(images[0]); i++)
    {
        is_FreeImageMem (mCamera, (char *)images[i].buffer, images[i].imageID);
    }


    return true;
}


BufferDescriptorType* UEyeCameraDescriptor::getDescriptorByAddress (char* pbuf)
{
    if (!pbuf)
        return 0;

    for (unsigned int i = 0; i < IMAGE_BUFFER_COUNT; i++)
    {
        if (images[i].buffer == (uint8_t *)pbuf)
            return &images[i];
    }

    return NULL;
}


double UEyeCameraDescriptor::getTemperature()
{
    IS_DEVICE_INFO deviceInfo;
    memset(&deviceInfo, 0, sizeof(IS_DEVICE_INFO));
    is_DeviceInfo(deviceID | IS_USE_DEVICE_ID, IS_DEVICE_INFO_CMD_GET_DEVICE_INFO, (void*)&deviceInfo, sizeof(deviceInfo));
    uint32_t temp = deviceInfo.infoDevHeartbeat.wTemperature;
    bool sign = (temp & 0x8000) != 0;
    int  deg  = (temp & 0x07F0) >> 4;
    int  part = (temp & 0x000F);

    double result = (sign ? -1 : 1) * (deg + (part / 16.0));
    //printf("devid=%d; Raw temp 0x%x, decoded: %lf\n",deviceID, temp, result);
    return result;
}


int UEyeCameraDescriptor::initBuffer()
{
    int width = 800, height = 600;
    // init the memorybuffer properties
    memset(&bufferProps, 0, sizeof(bufferProps));

    IS_RECT rectAOI;
    INT ret = is_AOI(mCamera, IS_AOI_IMAGE_GET_AOI, (void*)&rectAOI, sizeof(rectAOI));
    if (ret != IS_SUCCESS) {
        SYNC_PRINT(("UEyeCameraDescriptor::initBuffer(): is_AOI failed\n"));
        return 0;
    }

    width  = rectAOI.s32Width;
    height = rectAOI.s32Height;

    // get current colormode
    int colormode = is_SetColorMode(mCamera, IS_GET_COLOR_MODE);
    SYNC_PRINT(("UEyeCameraDescriptor::initBuffer(): SDK colormode: %d\n", colormode));

    // fill memorybuffer properties
    memset (&bufferProps, 0, sizeof(bufferProps));
    bufferProps.width  = width;
    bufferProps.height = height;
    bufferProps.colorformat = colormode;
    bufferProps.bitspp = getBitsPerPixel(colormode);
    bufferProps.imgformat = getQImageFormat(colormode);

    std::cout << "Expected buffer format" << std::endl;
    std::cout << bufferProps << std::endl;

    // Reallocate image buffers
    allocImages();
    return 0;
}

/*
uint16_t* UEyeCameraDescriptor::getFrame()
{
    INT dummy = 0;
    char *pBuffer = NULL, *pMem = NULL;

    is_GetActSeqBuf(mCamera, &dummy, &pMem, &pBuffer);

    int num;
    INT ret = IS_SUCCESS;

    if (!pBuffer || bufferProps.width < 1 || bufferProps.height < 1)
        return NULL;

    num = getImageNum (pBuffer);
    ret = is_LockSeqBuf (mCamera, num, pBuffer);

    int colormode = is_SetColorMode(mCamera, IS_GET_COLOR_MODE);

    qDebug("color mode is %d", colormode);

    uint16_t *pBufferG12 = new uint16_t[bufferProps.height * bufferProps.width];

    for (int i=0; i < bufferProps.height; i++)
    {
        uint16_t *data = (uint16_t *)pBufferG12 + (bufferProps.width * i);
        uint16_t *source = ((uint16_t *)pBuffer) + (bufferProps.width * i);
        for (int j=0; j < bufferProps.width; j++)
        {
            uint16_t pixel = *source;
            pixel =  (pixel >> 4) & 0xFF;
            uint16_t result = (0xFF << 24)| (pixel << 16) | (pixel << 8) | pixel;
            *data = result;
            source++;
            data++;
        }
    }

    ret |= is_UnlockSeqBuf (mCamera, num, pBuffer);

    return pBufferG12;
}*/

int UEyeCameraDescriptor::searchDefImageFormats(int suppportMask)
{
    int ret = IS_SUCCESS;
    int nNumber;
    int format = 0;
    IMAGE_FORMAT_LIST *pFormatList;

    if ((ret = is_ImageFormat(mCamera, IMGFRMT_CMD_GET_NUM_ENTRIES, (void*)&nNumber, sizeof(nNumber))) == IS_SUCCESS)
    {
        int i = 0;
        int nSize = sizeof(IMAGE_FORMAT_LIST) + (nNumber - 1) * sizeof(IMAGE_FORMAT_LIST);
        pFormatList = (IMAGE_FORMAT_LIST*)(new char[nSize]);
        pFormatList->nNumListElements = nNumber;
        pFormatList->nSizeOfListEntry = sizeof(IMAGE_FORMAT_INFO);

        if ((ret = is_ImageFormat(mCamera, IMGFRMT_CMD_GET_LIST, (void*)pFormatList, nSize)) == IS_SUCCESS)
        {
            for (i = 0; i < nNumber; i++)
            {
                if (pFormatList->FormatInfo[i].nSupportedCaptureModes & suppportMask)
                {
                    format = pFormatList->FormatInfo[i].nFormatID;
                    break;
                }
            }
        }
        else
        {
            qDebug("error: is_ImageFormat returned %d", ret);
        }

        delete[] (pFormatList);
    }
    else
    {
        qDebug("error: is_ImageFormat returned %d", ret);
    }
    qDebug("searchDefImageFormats returned %d [mask: 0x%X]\n", format, suppportMask);
    return format;
}


int UEyeCameraDescriptor::init(int deviceID, int binning, bool globalShutter, int pixelClock, double fps, bool isRgb)
{
    qDebug("Entering: init(deviceID=%d, %d)", deviceID, binning);
    if (deviceID == -1) {
        SYNC_PRINT(("Not initializing camera\n"));
        return IS_NO_SUCCESS;
    }

    HIDS cam = (HIDS) (deviceID | IS_USE_DEVICE_ID);

    unsigned int initialParameterSet = IS_CONFIG_INITIAL_PARAMETERSET_NONE;

    qDebug("  Calling is_InitCamera(cam=%d, NULL)", cam);
    int ret = is_InitCamera (&cam, 0);
    //INT ret = is_GetCameraInfo (m_hCamera, &m_CamInfo);

    if (ret != IS_SUCCESS)
    {
        char * str = 0;
        is_GetError(1,&ret,&str);
        qDebug("  Init is not OK for id %d. Error code %d. Error string: %s\n", deviceID, ret, str);
    }
    else {
        qDebug("  Init is OK");
        if (initialParameterSet == IS_CONFIG_INITIAL_PARAMETERSET_NONE)
        {
            ret = is_ResetToDefault (cam);
        }

        //int colormode = IS_CM_MONO12; //0;
        int colormode = IS_CM_MONO16;
        if (isRgb) {
            colormode = IS_CM_RGB8_PACKED;
        }
        if (is_SetColorMode (cam, colormode) != IS_SUCCESS)
        {
            qDebug("  Can't set color mode");
        } else {
            qDebug("  Color mode is set");
        }

        if (binning == 2) {
            qDebug("   Setting 2x2  binning...");
            is_SetBinning (cam, IS_BINNING_2X_VERTICAL | IS_BINNING_2X_HORIZONTAL);
        }

        if (binning == 4) {
            qDebug("   Setting 4x4  binning...");
            is_SetBinning (cam, IS_BINNING_4X_VERTICAL | IS_BINNING_4X_HORIZONTAL);
        }

        int result = is_SetRopEffect(cam, IS_SET_ROP_NONE, 0, 0);
        if (result != IS_SUCCESS)
        {
            qDebug("   PROBLEM...");
            char * str = 0;
            is_GetError(1,&result,&str);
            qDebug("   %s", str);

        }


        /* Shutter block */
        unsigned shutter;
        is_DeviceFeature (cam, IS_DEVICE_FEATURE_CMD_GET_SHUTTER_MODE, &shutter, sizeof(shutter));
        const char *strShutter="unknown";
        if (shutter == IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING) strShutter = "rolling";
        if (shutter == IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL) strShutter = "global";
        printf("   Current shutter state:%s\n", strShutter);

        if (globalShutter) {
            shutter = IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_GLOBAL;
        } else {
            shutter = IS_DEVICE_FEATURE_CAP_SHUTTER_MODE_ROLLING;
        }
        printf("   Setting to:%s\n", globalShutter ? "global" : "rolling");
        is_DeviceFeature (cam, IS_DEVICE_FEATURE_CMD_SET_SHUTTER_MODE, &shutter, sizeof(shutter));


        qDebug("   Setting pixelclock to %dMHz", pixelClock);
        is_PixelClock(cam, IS_PIXELCLOCK_CMD_SET, (void*)&pixelClock, sizeof(pixelClock));
        is_PixelClock(cam, IS_PIXELCLOCK_CMD_GET, (void*)&pixelClock, sizeof(pixelClock));
        qDebug("   New pixelclock is: %dMHz", pixelClock);



        qDebug("   Setting fps to %lffps", fps);
        is_SetFrameRate (cam, fps, &fps);
        qDebug("   New fps is: %lf", fps);

    }

    this->mCamera = cam;
    this->deviceID = deviceID;
    this->inited = true;

    /* get some special camera properties */
    ZeroMemory (&cameraProps, sizeof(cameraProps));

    // If the camera does not support a continuous AOI -> it uses special image formats
    cameraProps.usesImageFormats = false;
    INT nAOISupported = 0;
    if (is_ImageFormat(mCamera, IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED, (void*)&nAOISupported,
                       sizeof(nAOISupported)) == IS_SUCCESS)
    {
        cameraProps.usesImageFormats = (nAOISupported == 0);
    }

    /* set the default image format, if used */
    if (cameraProps.usesImageFormats)
    {
        // search the default formats
        cameraProps.imgFmtNormal  = searchDefImageFormats(CAPTMODE_FREERUN | CAPTMODE_SINGLE);
        cameraProps.imgFmtTrigger = searchDefImageFormats(CAPTMODE_TRIGGER_SOFT_SINGLE);
        // set the default formats
        if ((ret = is_ImageFormat(mCamera, IMGFRMT_CMD_SET_FORMAT, (void*)&cameraProps.imgFmtNormal,
                           sizeof(cameraProps.imgFmtNormal))) == IS_SUCCESS)
        {
            //m_nImageFormat = nFormat;
            //bRet = TRUE;///
        }
    }

    return ret;
}


/** This functions fixes the difference in windows and linux uEye API*/
int UEyeCameraDescriptor::waitUEyeFrameEvent(int timeout)
{
//    SYNC_PRINT(("UEyeCameraDescriptor::waitUEyeFrameEvent() for: %d\n", mCamera ));
#ifdef Q_OS_LINUX
    int result = is_WaitEvent (mCamera, IS_SET_EVENT_FRAME, timeout);
    if (result == IS_TIMED_OUT) {
        SYNC_PRINT(("UEyeCameraDescriptor::waitUEyeFrameEvent() is_WaitEvent timeouted\n"));
    } else {
        if (result != IS_SUCCESS) {
            SYNC_PRINT(("UEyeCameraDescriptor::waitUEyeFrameEvent() is_WaitEvent failed\n"));
        }
    }
    return result;
#endif
#ifdef Q_OS_WIN
    DWORD dwRet = WaitForSingleObject(mWinEvent, timeout);
    if (dwRet == WAIT_TIMEOUT)
        return IS_TIMED_OUT;
    if (dwRet == WAIT_OBJECT_0)
        return IS_SUCCESS;
    return  IS_NO_SUCCESS;
#endif
}
