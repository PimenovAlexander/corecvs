#include "directShow.h"


const char* DirectShowCameraDescriptor::codec_names[] =
{
        "yuv",
        "rgb",
        "mjpeg",
        "mjpeg fast decoder"
};

void DirectShowCameraDescriptor::setFromCameraParam(CaptureParameter &param,CameraParameter &camParam)
{
    param = CaptureParameter();
    param.setActive      (true);
    param.setMaximum     (camParam.maxValue);
    param.setMinimum     (camParam.minValue);
    param.setStep        (camParam.step);
    param.setDefaultValue(camParam.defaultValue);
}



int DirectShowCameraDescriptor::queryCameraParameters(CameraParameters &parameters)
{
    CameraParameter paramsFromCamera;

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Exposure, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::EXPOSURE], paramsFromCamera);

    CaptureParameter &expAuto = parameters.mCameraControls[CameraParameters::EXPOSURE_AUTO];
    expAuto = CaptureParameter();
    expAuto.setActive(true);
    expAuto.setIsMenu(0);
    expAuto.setMinimum(0);
    expAuto.setMaximum(1);
    expAuto.setStep(1);
    expAuto.setDefaultValue(0);
    expAuto.pushMenuItem(QString("OFF"), 0);
    expAuto.pushMenuItem(QString("ON") , 1);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Iris, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::IRIS], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Focus, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::FOCUS], paramsFromCamera);

    CaptureParameter &focusAuto = parameters.mCameraControls[CameraParameters::AUTO_FOCUS];
    focusAuto = CaptureParameter();
    focusAuto.setActive(true);
    focusAuto.setIsMenu(0);
    focusAuto.setMinimum(0);
    focusAuto.setMaximum(1);
    focusAuto.setStep(1);
    focusAuto.setDefaultValue(0);
    focusAuto.pushMenuItem(QString("OFF"), 0);
    focusAuto.pushMenuItem(QString("ON") , 1);

    /*Amp contols*/

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Brightness, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::BRIGHTNESS], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Contrast, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::CONTRAST], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Hue, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::HUE], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Saturation, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::SATURATION], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Sharpness, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::SHARPNESS], paramsFromCamera);

//    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_ColorEnable, &paramsFromCamera);
//    setFromCameraParam(parameters.mCameraControls[CameraParameters::COL], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_WhiteBalance, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::WHITE_BALANCE], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_BacklightCompensation, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::BACKLIGHT_COMPENSATION], paramsFromCamera);

    DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Gain, &paramsFromCamera);
    setFromCameraParam(parameters.mCameraControls[CameraParameters::GAIN], paramsFromCamera);

    return 0;
}

int DirectShowCameraDescriptor::setCaptureProperty(int id, int value)
{
    switch (id)
    {
        case CameraParameters::EXPOSURE_AUTO:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);
            param.isAuto = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);

            break;
        }

        case CameraParameters::EXPOSURE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);
            break;
        }

        case CameraParameters::IRIS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Iris, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_Control_Iris, &param);
            break;
        }

        case CameraParameters::FOCUS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);
            break;
        }

        case CameraParameters::AUTO_FOCUS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);
            param.isAuto = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);

            break;
        }

        /* AMP */
        case CameraParameters::CONTRAST:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Contrast, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_Contrast, &param);
            break;
        }

        case CameraParameters::HUE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Hue, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_Hue, &param);
            break;
        }

        case CameraParameters::SATURATION:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Saturation, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_Saturation, &param);
            break;
        }
        case CameraParameters::SHARPNESS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Sharpness, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_Sharpness, &param);
            break;
        }

        case CameraParameters::WHITE_BALANCE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_WhiteBalance, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_WhiteBalance, &param);
            break;
        }

        case CameraParameters::BACKLIGHT_COMPENSATION:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_BacklightCompensation, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_BacklightCompensation, &param);
            break;
        }

        case CameraParameters::GAIN:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Gain, &param);
            param.value = value;
            DirectShowCapDll_setCameraProp(deviceHandle, CAPDLL_AMP_Gain, &param);
            break;
        }

        default:
            {
                printf("Set request for unknown parameter (%d)\n", id);
                return 1;
            }
    }

    return 0;
}


int DirectShowCameraDescriptor::getCaptureProperty(int id, int *value)
{
    switch (id)
    {
        case CameraParameters::EXPOSURE_AUTO:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);
            *value = param.isAuto;
            break;
        }

        case CameraParameters::EXPOSURE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Exposure, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::IRIS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Iris, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::FOCUS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::AUTO_FOCUS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_Control_Focus, &param);
            *value = param.isAuto;
            break;
        }

        /* AMP */
        case CameraParameters::CONTRAST:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Contrast, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::HUE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Hue, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::SATURATION:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Saturation, &param);
            *value = param.value;
            break;
        }
        case CameraParameters::SHARPNESS:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Sharpness, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::WHITE_BALANCE:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_WhiteBalance, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::BACKLIGHT_COMPENSATION:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_BacklightCompensation, &param);
            *value = param.value;
            break;
        }

        case CameraParameters::GAIN:
        {
            CameraParameter param;
            DirectShowCapDll_getCameraProp(deviceHandle, CAPDLL_AMP_Gain, &param);
            *value = param.value;
            break;
        }

        default:
        {
            printf("Set request for unknown parameter (%d)\n", id);
            return 1;
        }
    }

    return 0;
}
