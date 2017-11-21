/*
 * V4l2.cpp
 *
 *  Created on: Jul 21, 2012
 *      Author: alexander
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

#include "V4L2.h"

//#define PROFILE_DEQUEUE
#ifdef PROFILE_DEQUEUE
#define TRACE_DEQUEUE(X) printf X
#else
#define TRACE_DEQUEUE(X)
#endif


const char *V4L2CameraDescriptor::STATE_NAMES[] =
{
    "UNINITIALIZED",
    "STOPPED",
    "STARTED"
};

STATIC_ASSERT(CORE_COUNT_OF(V4L2CameraDescriptor::STATE_NAMES) == V4L2CameraDescriptor::STATE_NUMBER, wrong_state_names_number)

int V4L2CameraDescriptor::initCamera(string deviceName, const V4L2CameraMode &mode)
{
    return initCamera(deviceName, mode.height, mode.width, mode.fpsnum, mode.fpsdenum, mode.compressed);
}

int V4L2CameraDescriptor::initCamera(
        string deviceName,
        int h,
        int w,
        int fpsnum,
        int fpsdenum,
        bool compressed)
{
    camFileName = std::string(deviceName);

    deviceHandle = open( deviceName.c_str(), O_RDWR );
    if (deviceHandle == -1)
    {
        SYNC_PRINT(("Video open error for device %s\n", deviceName.c_str()));
        deviceHandle = INVALID_HANDLE;
        return 1;
    }

    /* Setting input format */
    struct v4l2_format format;
    memset (&format, 0, sizeof (struct v4l2_format));
    format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    format.fmt.pix.width  = w;
    format.fmt.pix.height = h;
    format.fmt.pix.pixelformat = compressed ? V4L2_PIX_FMT_MJPEG: V4L2_PIX_FMT_YUYV;
    format.fmt.pix.field = V4L2_FIELD_ANY;

    if (ioctl (deviceHandle, VIDIOC_S_FMT, &format) == -1) {
        SYNC_PRINT(("Unable to set %d format for camera %s. Error: %s\n", V4L2_BUF_TYPE_VIDEO_CAPTURE, deviceName.c_str(), strerror(errno)));
        return 2;
    }

    formatH = format.fmt.pix.height;
    formatW = format.fmt.pix.width;
    SYNC_PRINT(("Set dimensions [%d x %d] for camera %s\n", format.fmt.pix.width, format.fmt.pix.height, deviceName.c_str()));

    /* Setting up FPS */
    struct v4l2_streamparm streamparm;
    memset (&streamparm, 0, sizeof (streamparm ));

    streamparm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    streamparm.parm.capture.timeperframe.numerator = fpsnum;
    streamparm.parm.capture.timeperframe.denominator = fpsdenum;

    SYNC_PRINT(("Setting fps [%d / %d] for camera %s\n",
                streamparm.parm.capture.timeperframe.numerator,
                streamparm.parm.capture.timeperframe.denominator,
                deviceName.c_str()));


    if(ioctl( deviceHandle, VIDIOC_S_PARM, &streamparm) == -1)
    {
        SYNC_PRINT(("Unable to set FPS for camera %s \n", deviceName.c_str()));
        return 3;
    }

    SYNC_PRINT(("Set fps [%d / %d] for camera %s\n",
                streamparm.parm.capture.timeperframe.numerator,
                streamparm.parm.capture.timeperframe.denominator,
                deviceName.c_str()));


    /*
    v4l2_control req;
    req.id = V4L2_CID_EXPOSURE_AUTO;
    req.value = V4L2_EXPOSURE_APERTURE_PRIORITY;
    if (ioctl(deviceID, VIDIOC_S_CTRL, &req))
        printf("VIDIOC_S_CTRL for V4L2_CID_EXPOSURE_AUTO failed. (%s) for %s\n", strerror(errno), deviceName.c_str());

    req.id = V4L2_CID_AUTOGAIN;
    req.value = true;
    if (ioctl(deviceID, VIDIOC_S_CTRL, &req))
        printf("VIDIOC_S_CTRL for V4L2_CID_AUTOGAIN failed. (%s) for %s\n", strerror(errno), deviceName.c_str());
    */
    return 0;
}

int V4L2CameraDescriptor::deinitCamera()
{
    if (deviceHandle == INVALID_HANDLE) {
        return 0;
    }
    SYNC_PRINT(("V4L2CameraDescriptor::deinitCamera(): closing camera %s \n", camFileName.c_str()));
    int result = close(deviceHandle);
    if (result == -1) {
        SYNC_PRINT(("V4L2CameraDescriptor::deinitCamera(): Unable to close handle for camera %s \n", camFileName.c_str()));
        return 1;
    }
    deviceHandle = INVALID_HANDLE;
    return 0;
}

int V4L2CameraDescriptor::initBuffers(unsigned loopQueueBufferNumber, unsigned loopQueueBufferNumberMin)
{
    /* Now we need to create buffers and tell driver about them */
    struct v4l2_requestbuffers reqbuf;

    memset (&reqbuf, 0, sizeof (reqbuf));
    reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuf.memory = V4L2_MEMORY_MMAP;
    reqbuf.count = loopQueueBufferNumber;

    if (ioctl (deviceHandle, VIDIOC_REQBUFS, &reqbuf) == -1)
    {
        printf ("Unable to allocate buffers in kernel for camera %s (handle 0x%X)"
                ". VIDIOC_REQBUFS failure (%d): %s.\n",
                camFileName.c_str(),
                deviceHandle,
                errno,
                strerror(errno));
        return -1;
    }

    /* We want at least five buffers. */
    if (reqbuf.count < loopQueueBufferNumberMin)
    {
        printf ("Not enough buffer memory (requested %d got %d)in kernel for camera %s (handle 0x%X)\n",
                loopQueueBufferNumberMin,
                reqbuf.count,
                camFileName.c_str(),
                deviceHandle
                );
        return -1;
    }

    printf("Allocated %d kernel buffers for camera %s (handle 0x%X)\n",
           reqbuf.count,
           camFileName.c_str(),
           deviceHandle);

    allocBuffers(reqbuf.count);

    /*Map kernel buffers to user side ones */
    for (unsigned i = 0; i < count; i++)
    {
        struct v4l2_buffer buffer;
        memset (&buffer, 0, sizeof (buffer));
        buffer.type = reqbuf.type;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;

        if (ioctl (deviceHandle, VIDIOC_QUERYBUF, &buffer) == -1)
        {
            printf ("VIDIOC_QUERYBUF request failed\n");
            return -1;
        }

        buffers[i].length = buffer.length; /* remember for munmap() */

        void *ptr = mmap (NULL, buffer.length,
                          PROT_READ | PROT_WRITE, /* recommended */
                          MAP_SHARED,             /* recommended */
                          deviceHandle, buffer.m.offset);

        if (ptr == MAP_FAILED)
        {
            printf ("Memmap failed\n");
            return -1;
        }

        buffers[i].start = ptr;
    }

    /* Queue the buffers. */
    for (unsigned i = 0; i < count; i++) {
        struct v4l2_buffer buffer;
        memset (&buffer, 0, sizeof (struct v4l2_buffer));
        buffer.index = i;
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        if (ioctl (deviceHandle, VIDIOC_QBUF, &buffer) == -1)
        {
            printf ("Unable to queue buffer (%d).\n", errno);
            return -1;
        }
    }

    queued = count;
    state = STOPPED;

    /* TODO: release resources on failure */

    return 0;

}

int V4L2CameraDescriptor::deinitBuffers()
{
    for (unsigned i = 0; i < count; i++)
    {
        V4L2UsersideBuffer *buffer = &(buffers[i]);
        if (buffer->start == NULL) {
            continue;
        }

        int result = munmap(buffer->start, buffer->length);
        if (result == -1)
        {
            SYNC_PRINT(("V4L2CameraDescriptor::deinitCamera(): Unable to close handle for camera %s \n", camFileName.c_str()));
            return 1;
        }
        buffer->length = 0;
        buffer->start  = NULL;
    }
    count = 0;
    return 0;
}

V4L2CameraDescriptor::~V4L2CameraDescriptor()
{
    stop();
    SYNC_PRINT(("V4L2CameraDescriptor::Deiniting camera structures\n"));
    deinitBuffers();
    SYNC_PRINT(("V4L2CameraDescriptor::Closing cameras\n"));
    deinitCamera();
    deletearr_safe(buffers);
}

int V4L2CameraDescriptor::start()
{
    if (state != STOPPED) {
        SYNC_PRINT(("V4L2CameraDescriptor::Trying to start camera <%s> in state <%s>\n", camFileName.c_str(), STATE_NAMES[state]));
        return -1;
    }

    uint32_t type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //printf ( "Device 0x%X is about to start video capture.\n", cam->deviceHandle);
    if (ioctl (deviceHandle, VIDIOC_STREAMON, &type) == -1) {
        printf ( "Unable to start capture: %d on camera %s (handle 0x%X):%s\n",
                errno,
                camFileName.c_str(),
                deviceHandle,
                strerror(errno));
        return -1;
    }

    state = STARTED;

    return 0;
}

int V4L2CameraDescriptor::stop()
{
    if (deviceHandle == INVALID_HANDLE) {
        return 0;
    }

    if (state != STARTED) {
        SYNC_PRINT(("V4L2CameraDescriptor::Trying to stop camera that is in state <%s>\n", STATE_NAMES[state]));
        return -1;
    }

    uint32_t type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    //printf ( "Device 0x%X is about to stop video capture.\n", cam->deviceHandle);
    if (ioctl (deviceHandle, VIDIOC_STREAMOFF, &type) == -1) {
        printf ( "Unable to stop capture: %d on camera %s (handle 0x%X).\n",
                 errno,
                 camFileName.c_str(),
                 deviceHandle);
        return -1;
    }

    state = STOPPED;

    return 0;
}


int V4L2CameraDescriptor::dequeue( V4L2BufferDescriptor &bufferDescr)
{
    if (deviceHandle == INVALID_HANDLE) {
        return 1;
    }

    bufferDescr.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    bufferDescr.memory = V4L2_MEMORY_MMAP;
    bufferDescr.isFilled = false;
    if (ioctl (deviceHandle, VIDIOC_DQBUF, &bufferDescr) == -1)
    {
        printf ("Unable to dequeue buffer (%d) on camera %s(handle 0x%X). Error:%s\n",
                errno,
                camFileName.c_str(),
                deviceHandle,
                strerror(errno));
        return 1;
    }

    if (bufferDescr.index >= count)
    {
        printf("Dequeue returned buffer id %d (expcted number < %d) for camera %s(handle 0x%X).\n",
               bufferDescr.index, count,
               camFileName.c_str(),
               deviceHandle);
        return 1;
    }

    queued--;
    TRACE_DEQUEUE(("Dequeued %d (%x) [%d]\n", bufferDescr.index, deviceHandle, queued));

    bufferDescr.isFilled = true;
    return 0;
}

int V4L2CameraDescriptor::enqueue(V4L2BufferDescriptor buffer)
{
    if (deviceHandle == INVALID_HANDLE) {
        return 1;
    }

    if (!buffer.isFilled)
    {
        printf("V4L2CameraDescriptor::enqueue():Empty Buffer. Will not enqueue\n");
        return 0;
    }

    if (ioctl (deviceHandle, VIDIOC_QBUF, &buffer) == -1)
    {
        printf ("V4L2CameraDescriptor::enqueue():Unable to requeue buffer (%d).\n", errno);
        return 1;
    }

    queued++;
    TRACE_DEQUEUE(("Enqueued %d (%x) [%d]\n", buffer.index, deviceHandle,  queued));
    return 0;
}

/* Reading and setting parameters */

int V4L2CameraDescriptor::queryParmeter(const uint32_t propID, v4l2_queryctrl &request) const
{
    if (deviceHandle == INVALID_HANDLE) {
        return -1;
    }

    //printf("Querying the attributes of %s parameter\n", text);
    memset(&request, 0, sizeof (v4l2_queryctrl));
    request.id = propID;

    if (ioctl(deviceHandle, VIDIOC_QUERYCTRL, &request))
    {
        if (EINVAL == errno) {
            DOTRACE(("\tThe parameter %d is either non-existent or not supported.\n", propID));
        } else {
            printf("VIDIOC_QUERYCTRL request failed. (%s)\n", strerror(errno));
        }
        return -1;
    }
    return 0;
}


int V4L2CameraDescriptor::setParmeter(const uint32_t propID, int value) const
{
    if (deviceHandle == INVALID_HANDLE) {
        return -1;
    }

    v4l2_control request;

    request.id = propID;
    request.value = value;
    int result = ioctl( deviceHandle, VIDIOC_S_CTRL, &request);
    if ( result != 0 )
    {
        printf( "V4L2CameraDescriptor::setParmeter(): VIDIOC_S_CTRL for %d with value %d failed. (%s)\n", propID, value, strerror(errno) );
        return -1;
    }
    return 0;
}

int V4L2CameraDescriptor::getParmeter(const uint32_t propID, int *value) const
{
    if (deviceHandle == INVALID_HANDLE) {
        return -1;
    }

    v4l2_control request;

    request.id = propID;
    request.value = 0;
    int result = ioctl( deviceHandle, VIDIOC_G_CTRL, &request);
    if ( result != 0 )
    {
        printf( "V4L2CameraDescriptor::getParmeter():VIDIOC_S_CTRL for %d with value %d failed. (%s)\n", propID, request.value, strerror(errno) );
        return -1;
    }
    *value = request.value;
    return 0;
}


void V4L2CameraDescriptor::setFromQueryCtrl(CaptureParameter &param, v4l2_queryctrl &ctrl)
{
    param = CaptureParameter();
    param.setMinimum(ctrl.minimum);
    param.setMaximum(ctrl.maximum);
    param.setDefaultValue(ctrl.default_value);
    if (ctrl.type == V4L2_CTRL_TYPE_MENU)
    {
        param.setIsMenu(true);

        if (ctrl.id != V4L2_CID_EXPOSURE_AUTO)
        {
            /* Query menu items */
            for (int index = ctrl.minimum; index <= ctrl.maximum; index++ )
            {
                struct v4l2_querymenu menuRequest;
                memset(&menuRequest, 0, sizeof(menuRequest));
                menuRequest.id = ctrl.id;
                menuRequest.index = index;

                int result = ioctl( deviceHandle, VIDIOC_QUERYMENU, &menuRequest);
                if ( result != 0 )
                {
                    printf( "VIDIOC_QUERYMENU for %d with index %d failed. (%s)\n", ctrl.id, index, strerror(errno) );
                    return;
                } else {
                 //   printf( "       MenuItem: %s\n", menuRequest.name);
                    param.pushMenuItem(std::string((const char *)menuRequest.name), index);
                }
            }
        } else {
            printf( "   Menu V4L2_EXPOSURE\n");
            param.pushMenuItem(std::string("Manual Exposture"),             V4L2_EXPOSURE_MANUAL);
            param.pushMenuItem(std::string("Auto Exposture"),               V4L2_EXPOSURE_AUTO);
            param.pushMenuItem(std::string("Shutter Priotiry Exposture"),   V4L2_EXPOSURE_SHUTTER_PRIORITY);
            param.pushMenuItem(std::string("Apperture Priority Exposture"), V4L2_EXPOSURE_APERTURE_PRIORITY);

        }
    }

    if (ctrl.type == V4L2_CTRL_TYPE_BOOLEAN)
    {
        param.setIsMenu(true);
        param.pushMenuItem(std::string("False"), 0);
        param.pushMenuItem(std::string("True") , 1);
    }

    if (!(ctrl.flags & (V4L2_CTRL_FLAG_DISABLED | V4L2_CTRL_FLAG_GRABBED | V4L2_CTRL_FLAG_READ_ONLY | V4L2_CTRL_FLAG_INACTIVE)))
    {
        param.setActive(true);
    }
}


const V4L2CameraDescriptor::ParamIdMapStruct V4L2CameraDescriptor::paramIdMap[] =
{
        {V4L2_CID_EXPOSURE_ABSOLUTE, CameraParameters::EXPOSURE},
        {V4L2_CID_EXPOSURE_AUTO    , CameraParameters::EXPOSURE_AUTO},
        {V4L2_CID_EXPOSURE         , CameraParameters::EXPOSURE_REL},
        {V4L2_CID_GAIN             , CameraParameters::GAIN},
        {V4L2_CID_AUTOGAIN         , CameraParameters::GAIN_AUTO},
        {V4L2_CID_BRIGHTNESS       , CameraParameters::BRIGHTNESS},
        {V4L2_CID_CONTRAST         , CameraParameters::CONTRAST},
        {V4L2_CID_SATURATION       , CameraParameters::SATURATION},
        {V4L2_CID_HUE              , CameraParameters::HUE},
        {V4L2_CID_SHARPNESS        , CameraParameters::SHARPNESS},
        {V4L2_CID_GAMMA            , CameraParameters::GAMMA},
        {V4L2_CID_POWER_LINE_FREQUENCY     , CameraParameters::POWER_LINE_FREQUENCY},
        {V4L2_CID_BACKLIGHT_COMPENSATION   , CameraParameters::BACKLIGHT_COMPENSATION},
        {V4L2_CID_WHITE_BALANCE_TEMPERATURE, CameraParameters::WHITE_BALANCE},
        {V4L2_CID_AUTO_WHITE_BALANCE, CameraParameters::AUTO_WHITE_BALANCE},
        {V4L2_CID_HFLIP, CameraParameters::HORIZONTAL_FLIP},
        {V4L2_CID_VFLIP, CameraParameters::VERTICAL_FLIP},

        {V4L2_CID_FOCUS_ABSOLUTE, CameraParameters::FOCUS},
        {V4L2_CID_FOCUS_RELATIVE, CameraParameters::FOCUS_RELATIVE},
        {V4L2_CID_FOCUS_AUTO    , CameraParameters::AUTO_FOCUS},



};


STATIC_ASSERT(CORE_COUNT_OF(V4L2CameraDescriptor::paramIdMap) == V4L2CameraDescriptor::paramIdMapSize, wrong_array_size_constant)

uint32_t V4L2CameraDescriptor::getPropIdByVi(CameraParameters::CameraControls viId)
{
    for (int i = 0; i < paramIdMapSize; i++) {
        if (viId == paramIdMap[i].viParamId) {
            return paramIdMap[i].v4l2PropID;
        }
    }

    return 0;
}

CameraParameters::CameraControls  V4L2CameraDescriptor::getViIdByProp(uint32_t v4l2Id)
{
    for (int i = 0; i < paramIdMapSize; i++) {
        if (v4l2Id == paramIdMap[i].v4l2PropID) {
            return paramIdMap[i].viParamId;
        }
    }
    return CameraParameters::FIRST;
}


int V4L2CameraDescriptor::queryCameraParameters(CameraParameters &params)
{
    v4l2_queryctrl ctrl;

    for (int i = 0; i < paramIdMapSize; i++)
    {
        int result = queryParmeter(paramIdMap[i].v4l2PropID, ctrl);
        if (result == 0) {
            setFromQueryCtrl(params.mCameraControls[paramIdMap[i].viParamId], ctrl);
        } else {
            /*SYNC_PRINT(("\tThe parameter %d (%s) is either non-existent or not supported.\n",
                    paramIdMap[i].v4l2PropID,
                    CameraParameters::getName(paramIdMap[i].viParamId)));*/
        }
    }
    return true;
}

int V4L2CameraDescriptor::setCaptureProperty(int id, int value)
{
    uint32_t v4l2Id = getPropIdByVi((CameraParameters::CameraControls)id);
    if (setParmeter(v4l2Id, value) != 0)
    {
        SYNC_PRINT(("V4L2CameraDescriptor::setCaptureProperty(): Failed to set %s for device (%d)\n",
            CameraParameters::getName((CameraParameters::CameraControls)id),
            deviceHandle
            ));
    }
    return 0;
}

int V4L2CameraDescriptor::getCaptureProperty(int id, int *value)
{
    uint32_t v4l2Id = getPropIdByVi((CameraParameters::CameraControls)id);
    if(getParmeter(v4l2Id, value) != 0)
    {
        SYNC_PRINT(("V4L2CameraDescriptor::getCaptureProperty(): Failed to get %s for device (%d)\n",
            CameraParameters::getName((CameraParameters::CameraControls)id),
            deviceHandle
        ));
    }

    return 0;
}

int V4L2CameraDescriptor::getCaptureName(string &name)
{
    struct v4l2_capability video_cap;

    if (deviceHandle == INVALID_HANDLE || ioctl(deviceHandle, VIDIOC_QUERYCAP, &video_cap) == -1)
    {
        printf("Couldn't get name of camera with handle %d\n", deviceHandle);
        return -1;
    }
    name = (char *)video_cap.card;
    return 0;
}

int V4L2CameraDescriptor::getCaptureFormats(int *num, ImageCaptureInterface::CameraFormat *&formats)
{
    SYNC_PRINT(("V4L2CameraDescriptor::getCaptureFormats()"));
    if (deviceHandle == INVALID_HANDLE) {
        SYNC_PRINT(("V4L2CameraDescriptor::getCaptureFormats(): deviceHandle == INVALID_HANDLE"));
        return -1;
    }
    vector<ImageCaptureInterface::CameraFormat> cameraFormats;
    struct v4l2_fmtdesc fmt;

    memset(&fmt, 0, sizeof(fmt));
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    while (ioctl(deviceHandle, VIDIOC_ENUM_FMT, &fmt) == 0)
    {
        struct v4l2_frmsizeenum fsize;

        memset(&fsize, 0, sizeof(fsize));
        fsize.index = 0;
        fsize.pixel_format = fmt.pixelformat;
        while (ioctl(deviceHandle, VIDIOC_ENUM_FRAMESIZES, &fsize) == 0)
        {
            if (fsize.type != V4L2_FRMSIZE_TYPE_DISCRETE)
            {
                break;
            }
            struct v4l2_frmivalenum fival;

            memset(&fival, 0, sizeof(fival));
            fival.index = 0;
            fival.pixel_format = fmt.pixelformat;
            fival.width = fsize.discrete.width;
            fival.height = fsize.discrete.height;
            while (ioctl(deviceHandle, VIDIOC_ENUM_FRAMEINTERVALS, &fival) == 0)
            {
                if (fival.type != V4L2_FRMIVAL_TYPE_DISCRETE)
                {
                    break;
                }
                ImageCaptureInterface::CameraFormat cameraFormat;
                cameraFormat.height = fsize.discrete.height;
                cameraFormat.width = fsize.discrete.width;
                cameraFormat.fps = fival.discrete.denominator / fival.discrete.numerator;
                cameraFormats.push_back(cameraFormat);
                fival.index++;
            }
            fsize.index++;
        }
        fmt.index++;
    }
    *num = cameraFormats.size();
    formats = new ImageCaptureInterface::CameraFormat[cameraFormats.size()];
    for (unsigned i = 0; i < cameraFormats.size(); i ++)
    {
        formats[i] = cameraFormats[i];
    }
    return 0;
}

std::string V4L2CameraDescriptor::getSerialNumber()
{
    // First we need to locate the device on the bus

    struct stat buf;

   if (fstat(deviceHandle, &buf) == 1)
   {
       SYNC_PRINT(("fstat call failed with %s", strerror(errno)));
       return "none";
   }

   int major_id = major(buf.st_rdev);
   int minor_id = minor(buf.st_rdev);

   SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():"
               "device %s major %d minor %d\n",
               camFileName.c_str(),

               major_id,
               minor_id));

   char deviceSysPath[1024] = "";
   char linkPath[1024] = "";
   snprintf(deviceSysPath, CORE_COUNT_OF(deviceSysPath), "/sys/dev/char/%d:%d/device", major_id, minor_id);
   ssize_t err = readlink(deviceSysPath, linkPath, CORE_COUNT_OF(linkPath));
   if (err == -1)
   {
       SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():Opening device softlink failed\n"));
   }

   SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():Opening device softlink <%s> -> <%s>\n", deviceSysPath, linkPath));
   /* Going for particular interface to the device */
   snprintf(deviceSysPath, CORE_COUNT_OF(deviceSysPath), "/sys/dev/char/%d:%d/%s/../serial", major_id, minor_id, linkPath);

   std::string result;

   SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():USB device serial path <%s>\n", deviceSysPath));
   char *serial = NULL;
   size_t len;
   FILE *serialFile = fopen(deviceSysPath, "rt");
   if (serialFile != NULL)
   {
         err = getline(&serial, &len, serialFile);
         if (err == -1)
         {
             SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():Error reading the line\n"));
         }

         for (size_t i = 0; i < strlen(serial); i++)
         {
             if (serial[i] == '\n' || serial[i] == '\r' ) serial[i] = 0;
         }
         fclose(serialFile);


        SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():Serial <%s>\n", serial));
        result = serial;
        free(serial);
   } else {
        SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber():serialFile is NULL\n"));
   }

   char *usbData = basename(linkPath);
   SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber(): Usbpath <%s>\n", usbData));
   result = usbData;

   SYNC_PRINT(("V4L2CameraDescriptor::getSerialNumber(): returning <%s>\n", result.c_str()));
   return result;

}

