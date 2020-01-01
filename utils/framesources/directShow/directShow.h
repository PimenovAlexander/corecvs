#pragma once

#include <stdint.h>

#include "core/utils/global.h"

#include "framesources/imageCaptureInterface.h"
#include "framesources/cameraControlParameters.h" // CameraParameters

#include "../../../wrappers/directShow/lib64/capdll.h" // DSCapDeviceId, CAPTURE_FORMAT_TYPE

class DirectShowCameraDescriptor
{
public:
    static cchar*              codec_names[];
    static CAPTURE_FORMAT_TYPE codec_types[];
    static uint                codec_size;

    DSCapDeviceId   deviceHandle = -1;
    int             size = 0;
    uint64_t        decodeTime = 0;
    uint64_t        timestamp = 0;
    bool            gotBuffer = false;
    G12Buffer      *buffer = NULL;
    RGB24Buffer    *buffer24 = NULL;
    uint8_t        *rawBuffer = NULL;
    int             height = 0;
    int             width = 0;

    /* Codec descriptor */
    enum {
       UNCOMPRESSED_YUV = 0,
       UNCOMPRESSED_RGB = 1,
       UNCOMPRESSED_BAYER8 = 2,
       UNCOMPRESSED_UYVY = 3,
       COMPRESSED_JPEG = 4,
       COMPRESSED_FAST_JPEG = 5
    };

    DirectShowCameraDescriptor() {}

private:
    static void setFromCameraParam(CaptureParameter &param, CameraParameter &camParam);

public:
    int queryCameraParameters(CameraParameters &parameters);
    int setCaptureProperty(int id, int value);
    int getCaptureProperty(int id, int *value);
};

/* EOF */
