#pragma once
/**
 * \file ImageCaptureInterface.h
 * \brief Add Comment Here
 *
 * \date Mar 14, 2010
 * \author alexander
 */
#include <string>
#include <QtCore/QObject>

#include "g12Buffer.h"
#include "rgb24Buffer.h"

using namespace corecvs;

class CameraParameters;

struct frame_data_t {

    frame_data_t() :
        streamId(0),
        timestamp(0)
    {}

    frame_data_t(uint64_t _timestamp) :
        streamId(0),
        timestamp(_timestamp)
    {}

    frame_data_t(int _stream, uint64_t _timestamp) :
        streamId(_stream),
        timestamp(_timestamp)
    {}

    int streamId;
    uint64_t timestamp;

    bool operator <= (const frame_data_t &other)
    {
//        SYNC_PRINT(("frame_data_t::operator <= ()\n"));
        if ( streamId < other.streamId) {
            return true;
        }
        if ( streamId > other.streamId) {
            return false;
        }
        return (timestamp <= other.timestamp);
    }
};

class CaptureStatistics
{
public:
    static char const *names[];

    enum {
        DESYNC_TIME,
        INTERNAL_DESYNC_TIME,
        DECODING_TIME,
        CONVERTING_TIME,
        INTERFRAME_DELAY,
        MAX_TIME_ID,
        DATA_SIZE = MAX_TIME_ID,
        MAX_ID
    };

    FixedVector<uint64_t, MAX_ID> values;
    uint64_t framesSkipped;
    uint64_t triggerSkipped;
    double temperature[2];

    CaptureStatistics()
    {
        CORE_CLEAR_STRUCT(*this);
    }

    CaptureStatistics &operator +=(const CaptureStatistics &toAdd)
    {
        values += toAdd.values;
        framesSkipped  += toAdd.framesSkipped;
        triggerSkipped += toAdd.triggerSkipped;
        temperature[0] = toAdd.temperature[0];
        temperature[1] = toAdd.temperature[1];
        return *this;
    }

    CaptureStatistics &operator /= ( int frames )
    {
        values /= frames;
        return *this;
    }
};

//---------------------------------------------------------------------------

class ImageCaptureInterface : public QObject
{
    Q_OBJECT

public:
    enum CapErrorCode
    {
        SUCCESS       = 0,
        SUCCESS_1CAM  = 1,
        FAILURE      = 2
    };

    enum CapCameraId
    {
        CAMERA_LEFT   = 0x01,
        CAMERA_RIGHT  = 0x10,
        CAMERA_BOTH   = 0x11
    };

    /**
     *  This structure will hold the rational number
     *  for the FPS
     *
     **/
    typedef struct
    {
        uint16_t num;    /**< the numerator*/
        uint16_t denom;  /**< the denominator*/
    } fps_t;

    /**
     * Used to pass capture parameters in set/get requests
     *
     **/

    enum CapPropertyAccessType
    {
        ACCESS_NONE         = 0x00,
        ACCESS_READ         = 0x01,
        ACCESS_WRITE        = 0x10,
        ACCESS_READWRITE    = 0x11
    };

    /**
     *  This will hold the pair of frames
     **/
    class FramePair
    {
    public:
        G12Buffer* bufferRight;       /**< Pointer to right gray scale buffer*/
        G12Buffer* bufferLeft;        /**< Pointer to left  gray scale buffer*/
        RGB24Buffer *rgbBufferLeft;
        RGB24Buffer *rgbBufferRight;
        uint64_t   leftTimeStamp;    /**< Timestamp for left image */
        uint64_t   rightTimeStamp;   /**< Timestamp for right image */

        int streamId;

        FramePair(
            G12Buffer* _bufferLeft = NULL,
            G12Buffer* _bufferRight = NULL,
            RGB24Buffer *_rgbBufferLeft = NULL,
            RGB24Buffer *_rgbBufferRight = NULL
        ) :
              bufferRight(_bufferRight)
            , bufferLeft (_bufferLeft )
            , rgbBufferLeft(_rgbBufferLeft)
            , rgbBufferRight(_rgbBufferRight)
            , leftTimeStamp (0)
            , rightTimeStamp(0)
            , streamId(0)
        {}

        bool hasBoth() const    { return bufferLeft && bufferRight; }
    };

    static bool isRgb;

    struct CameraFormat
    {
        int fps;
        int width;
        int height;
    };

signals:
    void    newFrameReady(frame_data_t frameData);
    void    newImageReady();

    void    newStatisticsReady(CaptureStatistics stats);
    void    streamPaused();

public:
    ImageCaptureInterface();
    virtual ~ImageCaptureInterface();

    /**
     *  Fabric to create particular implementation of the capturer
     **/
    static ImageCaptureInterface *fabric(string input, bool isRgb = false);
    /**
     *  Main function to request frames from image interface
     **/
    virtual FramePair    getFrame() = 0;
    virtual FramePair    getFrameRGB24()
    {
        return getFrame();
    }

    virtual CapErrorCode setCaptureProperty(int id, int value);

    virtual CapErrorCode getCaptureProperty(int id, int *value);

    virtual CapErrorCode getCaptureName(QString &value);

    virtual CapErrorCode getFormats(int *num, CameraFormat *&);

    /**
     * Return the interface device string
     **/
    virtual QString      getInterfaceName();
    /**
     * Return some id of one of the stereo-/multi- camera sub-device
     **/
    virtual CapErrorCode getDeviceName(int num, QString &name);

    /**
     * Check if a specific property can be set/read and what values it can take
     **/

    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter);

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();
    virtual CapErrorCode pauseCapture();
    virtual CapErrorCode nextFrame();

    /**
     * Check if particular image capturer support pausing.
     * This function can be called before starting or even initing the capture
     **/
    virtual bool         supportPause();

protected:
    /**
     *  Internal function to trigger frame notify signal
     **/
    virtual void notifyAboutNewFrame(frame_data_t frameData);


};
