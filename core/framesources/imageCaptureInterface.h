#pragma once
/**
 * \file ImageCaptureInterface.h
 * \brief Add Comment Here
 *
 * \date Mar 14, 2010
 * \author alexander
 */
#include <string>


#include "core/buffers/g12Buffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"

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

/**
 *  Callback style without Qt slots and signals
 *
 *  The target here is to prepare separation from Qt. So far we have to use virtual multiple inheritance.
 **/
class ImageInterfaceReceiver
{
public:
    virtual void newFrameReadyCallback(frame_data_t frameData) = 0;
    virtual void newImageReadyCallback() = 0;
    virtual void newStatisticsReadyCallback(CaptureStatistics stats) = 0;
    virtual void streamPausedCallback() = 0;
    virtual void newCameraParamValueCallBack(int idParam) = 0;
};

class ImageCaptureInterface
{
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
        THIRD_FRAME,
        FOURTH_FRAME,
        MAX_INPUTS_NUMBER
    };

    static inline const char *getFrameSourceName(const FrameSourceId &value)
    {
        switch (value)
        {
            case LEFT_FRAME  :   return "LEFT_FRAME";
            case RIGHT_FRAME :   return "RIGHT_FRAME";
            case THIRD_FRAME :   return "THIRD_FRAME";
            case FOURTH_FRAME:   return "FOURTH_FRAME";
            default:            break;
        }
        return "Not in range";
    }

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
        struct FrameData {
            G12Buffer    *g12Buffer = NULL;
            RGB24Buffer  *rgbBuffer = NULL;
            uint64_t     timeStamp = 0;
        };

        FrameData buffers[MAX_INPUTS_NUMBER];

        G12Buffer   *bufferLeft()     const { return buffers[LEFT_FRAME ].g12Buffer;} /**< Pointer to left  gray scale buffer*/
        G12Buffer   *bufferRight()    const { return buffers[RIGHT_FRAME].g12Buffer;} /**< Pointer to right gray scale buffer*/
        RGB24Buffer *rgbBufferLeft()  const { return buffers[LEFT_FRAME ].rgbBuffer;}
        RGB24Buffer *rgbBufferRight() const { return buffers[RIGHT_FRAME].rgbBuffer;}

        RGB24Buffer *rgbBufferDefault() const { return buffers[DEFAULT_FRAME].rgbBuffer;}


        void setBufferLeft      (G12Buffer   *buf)  {  buffers[LEFT_FRAME  ].g12Buffer = buf;} /**< Pointer to left  gray scale buffer*/
        void setBufferRight     (G12Buffer   *buf)  {  buffers[RIGHT_FRAME ].g12Buffer = buf;} /**< Pointer to right gray scale buffer*/
        void setRgbBufferLeft   (RGB24Buffer *buf)  {  buffers[LEFT_FRAME  ].rgbBuffer = buf;}
        void setRgbBufferRight  (RGB24Buffer *buf)  {  buffers[RIGHT_FRAME ].rgbBuffer = buf;}

        void setRgbBufferDefault(RGB24Buffer *buf)  {  buffers[DEFAULT_FRAME].rgbBuffer = buf;}


        uint64_t     timeStampLeft()  { return buffers[LEFT_FRAME  ].timeStamp;}
        uint64_t     timeStampRight() { return buffers[RIGHT_FRAME ].timeStamp;}
        void setTimeStampLeft (uint64_t stamp)  { buffers[LEFT_FRAME ].timeStamp = stamp; }
        void setTimeStampRight(uint64_t stamp)  { buffers[RIGHT_FRAME].timeStamp = stamp; }




        FramePair(G12Buffer   *_bufferLeft     = NULL
                , G12Buffer   *_bufferRight    = NULL
                , RGB24Buffer *_rgbBufferLeft  = NULL
                , RGB24Buffer *_rgbBufferRight = NULL)            
        {
            buffers[LEFT_FRAME ].g12Buffer = _bufferLeft ;
            buffers[RIGHT_FRAME].g12Buffer = _bufferRight;

            buffers[LEFT_FRAME ].rgbBuffer = _rgbBufferLeft ;
            buffers[RIGHT_FRAME].rgbBuffer = _rgbBufferRight;
        }

        bool allocBuffers(uint height, uint width, bool shouldInit = false)
        {
            freeBuffers();
            for (int i = 0; i < MAX_INPUTS_NUMBER; i++)
            {
                buffers[i].g12Buffer  = new G12Buffer(height, width, shouldInit);
            }
            return hasBoth();
        }

        void freeBuffers()
        {
            for (int i = 0; i < MAX_INPUTS_NUMBER; i++)
            {
                delete_safe(buffers[i].g12Buffer);
                delete_safe(buffers[i].rgbBuffer);
            }
        }

        FramePair clone() const
        {
            FramePair result(new G12Buffer(bufferLeft()), new G12Buffer(bufferRight()));

            if (rgbBufferLeft() != NULL)
                result.buffers[LEFT_FRAME ].rgbBuffer = new RGB24Buffer(buffers[LEFT_FRAME ].rgbBuffer);
            if (buffers[RIGHT_FRAME].rgbBuffer != NULL)
                result.buffers[RIGHT_FRAME].rgbBuffer = new RGB24Buffer(buffers[RIGHT_FRAME].rgbBuffer);

            result.setTimeStampLeft (buffers[LEFT_FRAME ].timeStamp);
            result.setTimeStampRight(buffers[RIGHT_FRAME].timeStamp);
            return result;
        }


        bool        hasBoth() const         { return buffers[LEFT_FRAME].g12Buffer != NULL && buffers[RIGHT_FRAME].g12Buffer != NULL; }

        /* This is still legacy */
        uint64_t    timeStamp() const       { return buffers[LEFT_FRAME].timeStamp / 2 + buffers[RIGHT_FRAME].timeStamp / 2;    }
        int64_t     diffTimeStamps() const  { return buffers[LEFT_FRAME].timeStamp     - buffers[RIGHT_FRAME].timeStamp;        }
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

public:
    ImageCaptureInterface();
    virtual ~ImageCaptureInterface();

    /* Callback receiver */
    ImageInterfaceReceiver *imageInterfaceReceiver;

    /**
     *  Fabric to create particular implementation of the capturer
     **/
    static void getAllCameras(vector<std::string> &cameras);
//    static ImageCaptureInterface *fabric(string input, bool isRgb = false);
//    static ImageCaptureInterface *fabric(string input, int h, int w, int fps, bool isRgb = false);

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

    virtual CapErrorCode getCaptureName(std::string &value);

    /**
     *  Enumerates camera formats
     **/
    virtual CapErrorCode getFormats(int *num, CameraFormat *& formats);

    /**
     * After the init returns currently set format.
     * Children could implement this function ad lilbitum.
     * Call in uninted state results in undefinded behaviour.
     *
     *  \param format - place to output result
     *
     *  \return  true if the reuslt is valid, false othervize;
     **/
    virtual bool         getCurrentFormat(CameraFormat &format);

    /**
     * Return the interface device string
     **/
    virtual std::string      getInterfaceName();
    /**
     * Return some id of one of the stereo-/multi- camera sub-device
     **/
    virtual CapErrorCode getDeviceName(int num, std::string &name);

    /**
     * Return serial number stereo-/multi- camera sub-device if applicable
     **/
    virtual std::string  getDeviceSerial(int num = LEFT_FRAME);

    /**
     * Check if a specific property can be set/read and what values it can take
     **/
    virtual CapErrorCode queryCameraParameters(CameraParameters &parameter);

    /**
     * Initialize capture with the given before parameters.
     *
     * Actually set format can then be queried by the call to getCurrentFormat()
     **/
    virtual CapErrorCode initDevice(bool fullInit);
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

    virtual void notifyAboutNewCameraParamValue(int idParam);
};


class ImageCaptureInterfaceProducer
{
public:
    virtual std::string getPrefix() = 0;
    virtual ImageCaptureInterface *produce(std::string &name, bool isRGB) = 0;

};


template<class Target>
class ImageCaptureInterfaceProducerWrapper : public ImageCaptureInterfaceProducer
{
public:
    std::string prefix;

    ImageCaptureInterfaceProducerWrapper(const std::string &prefix) :
        prefix(prefix)
    {}

    virtual std::string getPrefix() override;
    virtual ImageCaptureInterface *produce(std::string &name, bool isRGB) override
    {
        return new Target(name, isRGB);
    }
};

class ImageCaptureInterfaceFabric
{
public:
    /** Public function to get the only one instance of this object.
     *  It will be automatically destroyed on shut down.
     */
    static ImageCaptureInterfaceFabric* getInstance();
    static void printCaps();


    std::vector<ImageCaptureInterfaceProducer *> producers;
    void addProducer(ImageCaptureInterfaceProducer *producer);

    ImageCaptureInterface *fabricate(const std::string &name, bool isRGB);

};




