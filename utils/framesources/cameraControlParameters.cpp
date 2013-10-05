#include "cameraControlParameters.h"


const char *CameraParameters::names[] = {
    "EXPOSURE_ABS",
    "EXPOSURE_AUTO",
    "EXPOSURE",
    "GAIN",
    "GAIN_AUTO",
    "GAIN_BOOST",
    "GAIN_RED",
    "GAIN_GREEN",
    "GAIN_BLUE",
    "IRIS",
    "FPS",
    "FOCUS",
    "ZOOM",
    "PIXELCLOCK",
    "HUE",
    "SATURATION",
    "SHARPNESS",
    "BRIGHTNESS",
    "CONTRAST",
    "GAMMA",
    "WHITE BALANCE",
    "AUTO WHITE BALANCE",
    "POWER LINE FREQUENCY",
    "BACKLIGHT COMPENSATION",
    "AUTO_FOCUS",

    "UEYE MASTER FLASH DELAY",
    "UEYE MASTER FLASH DURATION",
    "UEYE SLAVE TRIGGER DELAY",

    "HORIZONTAL_FLIP",
    "VERTICAL_FLIP"
};

STATIC_ASSERT(CORE_COUNT_OF(CameraParameters::names) == CameraParameters::LAST, wrong_camera_parameter_num);
