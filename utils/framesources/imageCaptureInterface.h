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

typedef struct {
    uint64_t timestamp;
} frame_data_t;

class CaptureStatistics
{
public:
    static char const *names[];

    enum {
        DESYNC_TIME,
        INTERNAL_DESYNC_TIME,
        DECODING_TIME,
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
        values         += toAdd.values;
        framesSkipped  += toAdd.framesSkipped;
        triggerSkipped += toAdd.triggerSkipped;
        temperature[0]  = toAdd.temperature[0];
        temperature[1]  = toAdd.temperature[1];
        return *this;
    }

    CaptureStatistics &operator /=(int frames)
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
    /**
     *   Error code for init functions
     **/
    enum CapErrorCode
    {
        SUCCESS       = 0,
        SUCCESS_1CAM  = 1,
        FAILURE       = 2
    };

    enum FrameSourceId {
        LEFT_FRAME,
        DEFAULT_FRAME = LEFT_FRAME,
        RIGHT_FRAME,
        MAX_INPUTS_NUMBER
    };

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
        G12Buffer   *bufferLeft;      /**< Pointer to left  gray scale buffer*/
        G12Buffer   *bufferRight;     /**< Pointer to right gray scale buffer*/
        RGB24Buffer *rgbBufferLeft;
        RGB24Buffer *rgbBufferRight;
        uint64_t     timeStampLeft;
        uint64_t     timeStampRight;

        FramePair(G12Buffer   *_bufferLeft     = NULL
                , G12Buffer   *_bufferRight    = NULL
                , RGB24Buffer *_rgbBufferLeft  = NULL
                , RGB24Buffer *_rgbBufferRight = NULL)
            : bufferLeft (_bufferLeft )
            , bufferRight(_bufferRight)
            , rgbBufferLeft(_rgbBufferLeft)
            , rgbBufferRight(_rgbBufferRight)
            , timeStampLeft(0)
            , timeStampRight(0)
        {}

        bool allocBuffers(uint height, uint width, bool shouldInit = false)
        {
            freeBuffers();
            bufferLeft  = new G12Buffer(height, width, shouldInit);
            bufferRight = new G12Buffer(height, width, shouldInit);
            return hasBoth();
        }

        void freeBuffers()
        {
            delete_safe(bufferLeft);
            delete_safe(bufferRight);
            delete_safe(rgbBufferLeft);
            delete_safe(rgbBufferRight);
        }

        FramePair clone() const
        {
            FramePair result(new G12Buffer(bufferLeft), new G12Buffer(bufferRight));
            if (rgbBufferLeft != NULL)
                result.rgbBufferLeft = new RGB24Buffer(rgbBufferLeft);
            if (rgbBufferRight != NULL)
                result.rgbBufferRight = new RGB24Buffer(rgbBufferRight);
            result.timeStampLeft  = timeStampLeft;
            result.timeStampRight = timeStampRight;
            return result;
        }

        bool        hasBoth() const         { return bufferLeft != NULL && bufferRight != NULL; }
        uint64_t    timeStamp() const       { return timeStampLeft / 2 + timeStampRight / 2;    }
        int64_t     diffTimeStamps() const  { return timeStampLeft     - timeStampRight;        }
    };

    bool mIsRgb;                                            // flag that camera supports colors

    struct CameraFormat
    {
        int height;
        int width;
        int fps;

        CameraFormat(int h = 0, int w = 0, int f = 0) : height(h), width(w), fps(f) {}

        bool operator==(CameraFormat &fmt) const { return height == fmt.height && width == fmt.width && fps == fmt.fps; }
        bool operator!=(CameraFormat &fmt) const { return !operator==(fmt); }
        bool operator!()                   const { return height == 0 && width == 0 && fps == 0; }
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

    /**
     *  Enumerates camera formats
     **/
    virtual CapErrorCode getFormats(int *num, CameraFormat *& formats);

    /**
     * Return the interface device string
     **/
    virtual QString      getInterfaceName();
    /**
     * Return some id of one of the stereo-/multi- camera sub-device
     **/
    virtual CapErrorCode getDeviceName(int num, QString &name);

    /**
     * Return serial number stereo-/multi- camera sub-device if applicable
     **/
    virtual std::string  getDeviceSerial(int num = LEFT_FRAME);

    /**
     * Check if a specific property can be set/read and what values it can take
     **/
    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter);

    /**
     * Initialize capture with the given before parameters. If need it returns actually set format for cameras.
     **/
    virtual CapErrorCode initCapture(CameraFormat *actualFormat = NULL);

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
