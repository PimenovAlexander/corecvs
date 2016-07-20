#ifndef V4L2_H_
#define V4L2_H_
/*
 * V4L2.h
 *
 *  Created on: Jul 21, 2012
 *      Author: alexander
 */

#include <stdint.h>
#include <string.h>
#include <string>
#include <errno.h>
#include <linux/videodev2.h>


#include "cameraControlParameters.h"


using std::string;

/**
 *   This structure describes V4L2 user side buffer
 **/
class V4L2UsersideBuffer
{
public:
    void *start;   /**< Start address of the buffer*/
    size_t length; /**< Length of the buffer in bytes*/

    V4L2UsersideBuffer() :
        start(NULL),
        length(0)
    {}
};

/**
 * Buffer descriptor type is a wrapper around the Video 4 Linux
 * v4l2_buffer structure
 * */
class V4L2BufferDescriptor : public v4l2_buffer {
    public:
        bool isFilled; /**< Flag that is used to tell filled buffer from not filled */

        V4L2BufferDescriptor()
        {
            memset(this, 0, sizeof(V4L2BufferDescriptor));
        }

        uint64_t usecsTimeStamp()
        {
            return timestamp.tv_sec * 1000000UL + timestamp.tv_usec;
        }
};

class V4L2CameraMode {
public:
    V4L2CameraMode()
    {
    }

    int height;
    int width;
    int fpsnum;
    int fpsdenum;
    bool compressed;
};

/**
 * A wrapper over V4L2 to access the camera
 **/
class V4L2CameraDescriptor
{
public:
    static const unsigned LoopQueueBufferNumberDefault    = 8;
    static const unsigned LoopQueueBufferNumberMinDefault = 5;

    static const int INVALID_HANDLE = -1;

    enum State {
        UNINITIALIZED,
        STOPPED,
        STARTED,
        STATE_NUMBER
    };

    static const char *STATE_NAMES[];

    int state;
    string camFileName;           /**< name of the camera device (a UNIX path) */
    int deviceHandle;

    int formatH;
    int formatW;

    V4L2UsersideBuffer *buffers;
    unsigned count;               /**< number of buffers associated with current descriptor */
    unsigned queued;

    V4L2CameraDescriptor() :
        state(UNINITIALIZED),
        camFileName(""),
        deviceHandle(INVALID_HANDLE),
        formatH(0),
        formatW(0),
        buffers(NULL),
        count(0),
        queued(0)
    {}

    /**
     *   Allocates given number of userside buffer structures
     **/
    void allocBuffers(int _count)
    {
        count = _count;
        buffers = new V4L2UsersideBuffer[count];
    }

    int initCamera(string deviceName, int h, int w, int fpsnum, int fpsdenum, bool compressed);
    int initCamera(string deviceName, const V4L2CameraMode &mode);

    int initBuffers(
            unsigned loopQueueBufferNumber    = LoopQueueBufferNumberDefault,
            unsigned loopQueueBufferNumberMin = LoopQueueBufferNumberMinDefault);

private:
    int deinitCamera();
    int deinitBuffers();
public:

    int start();
    int stop();

    int dequeue(V4L2BufferDescriptor &buffer);
    int enqueue(V4L2BufferDescriptor buffer);

    int queryCameraParameters(CameraParameters &params);
    int setCaptureProperty(int id, int value);
    int getCaptureProperty(int id, int *value);
    int getCaptureName(string &name);
    int getCaptureFormats(int *num, ImageCaptureInterface::CameraFormat *&formats);

     std::string getSerialNumber();

private:
    int queryParmeter(const uint32_t propID, v4l2_queryctrl &request) const;
    int setParmeter  (const uint32_t propID, int  value) const;
    int getParmeter  (const uint32_t propID, int *value) const;

    void setFromQueryCtrl(CaptureParameter &param, v4l2_queryctrl &ctrl);

public:
    /* Probably should use map here */
    struct ParamIdMapStruct {
       uint32_t v4l2PropID;
       CameraParameters::CameraControls viParamId;
    };


   static const ParamIdMapStruct paramIdMap[];
   static const int paramIdMapSize = 20;

   static uint32_t getPropIdByVi(CameraParameters::CameraControls viId);
   static CameraParameters::CameraControls  getViIdByProp(uint32_t v4l2Id);


   ~V4L2CameraDescriptor();

};


#endif /* V4L2_H_ */
