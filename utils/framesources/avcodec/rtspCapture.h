#ifndef RTSPCAPTURE_H
#define RTSPCAPTURE_H

#include <QtCore/QQueue>
#include <QtCore/QSemaphore>
#include <QtCore/QThread>

extern "C" {

# if __WORDSIZE == 64
#  define INT64_C(c)	c ## L
# else
#  define INT64_C(c)	c ## LL
# endif
# if __WORDSIZE == 64
#  define UINT64_C(c)	c ## UL
# else
#  define UINT64_C(c)	c ## ULL
# endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#include "imageCaptureInterface.h"
#include "preciseTimer.h"

#define FRAME_CIRCULAR_BUFFER_SIZE 50

class RTSPCapture : public ImageCaptureInterface
{
public:
    RTSPCapture(const QString &params);
    ~RTSPCapture();

    virtual ImageCaptureInterface::CapErrorCode initCapture();
    virtual ImageCaptureInterface::CapErrorCode startCapture();
    virtual CapErrorCode pauseCapture();
    virtual CapErrorCode nextFrame();
    virtual bool         supportPause();
    virtual FramePair    getFrame();

    std::string mName;
    AVFormatContext* mFormatContext;
    AVCodecContext* mCodecContext;
    AVCodec* mCodec;

    bool mIsPaused;

    /* Codec related stuff */
    AVFrame* mFrame;
    AVPacket mPacket;

    unsigned mVideoStream;

    PreciseTimer mLastFrameTime;

private:
    class SpinThread : public QThread
    {
    public:
        RTSPCapture *mInstance;
    private:
        bool mFirstFrame; /* Need this to skip the first frame, which might break timestamps */

    public:
        SpinThread(RTSPCapture *instance) :
            mInstance(instance)
          , mFirstFrame(true)
        {}

        virtual void run (void);
    };

    class FrameCircularBuffer
    {
    public:
        RTSPCapture *mInstance;
    private:
        QQueue<FramePair> mFrames;
        QSemaphore mReadyFrames;
        QSemaphore mFreeFrames;

    public:

        FrameCircularBuffer(RTSPCapture *instance) :
            mInstance(instance)
          , mReadyFrames(0)
          , mFreeFrames(FRAME_CIRCULAR_BUFFER_SIZE)          
        {}

        int available() {return mReadyFrames.available();}
        uint64_t firstFrameTimestamp();
        uint64_t secondFrameTimestamp();
        FramePair dequeue();
        void enqueue(const FramePair& frame);

    };


public:
    SpinThread spin;      /**< Spin thread that runs circular buffer of frames */
    FrameCircularBuffer fcb; /* Frame Circular Buffer (FCB) */
};

#endif // RTSPCAPTURE_H
