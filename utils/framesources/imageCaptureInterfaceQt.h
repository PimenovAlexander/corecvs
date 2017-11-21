#ifndef IMAGECAPTUREINTERFACEQT_H
#define IMAGECAPTUREINTERFACEQT_H

#include <QtCore/QObject>

#include "imageCaptureInterface.h"


class ImageCaptureInterfaceQt;

class ImageCaptureInterfaceQtFactory {
public:
    static ImageCaptureInterfaceQt *fabric(string input, bool isRgb = false);
    static ImageCaptureInterfaceQt *fabric(string input, int h, int w, int fps, bool isRgb = false);

};

class ImageCaptureQtNotifier : public QObject
{
    Q_OBJECT
public:
signals:
    void    newFrameReady(frame_data_t frameData);
    void    newImageReady();
    void    newStatisticsReady(CaptureStatistics stats);
    void    streamPaused();

};

class ImageCaptureInterfaceQt: public ImageInterfaceReceiver, public ImageCaptureQtNotifier, public virtual ImageCaptureInterface
{
public:

    ImageCaptureInterfaceQt()
    {
        qRegisterMetaType<frame_data_t>("frame_data_t");
        imageInterfaceReceiver = this;
        SYNC_PRINT(("Constructing ImageCaptureInterfaceQt::ImageCaptureInterfaceQt()\n"));
    }

    virtual void newFrameReadyCallback(frame_data_t frameData) override
    {
        emit newFrameReady(frameData);
    }

    virtual void newImageReadyCallback() override
    {
        emit newImageReady();
    }

    virtual void newStatisticsReadyCallback(CaptureStatistics stats) override
    {
        emit newStatisticsReady(stats);
    }

    virtual void streamPausedCallback() override
    {
        emit streamPaused();
    }
};

template<class WrappeeType>
class ImageCaptureInterfaceWrapper : public virtual ImageCaptureInterfaceQt, public virtual WrappeeType
{
public:
    ImageCaptureInterfaceWrapper(const std::string &input, bool isRGB) :
        WrappeeType(input, isRGB)
    {}

    ImageCaptureInterfaceWrapper(const std::string &input, bool isVerbose, bool isRGB) :
        WrappeeType(input, isVerbose, isRGB)
    {}


    ImageCaptureInterfaceWrapper(const std::string &input) :
        WrappeeType(input)
    {}

    ImageCaptureInterfaceWrapper(const string &_devname, int h, int w, int fps, bool isRgb) :
        WrappeeType(_devname, h, w, fps, isRgb)
    {}

    ImageCaptureInterfaceWrapper(
        G12Buffer *_cl,
        G12Buffer *_cr,
        G12Buffer *_cln,
        G12Buffer *_crn ) : WrappeeType (_cl, _cr, _cln, _crn)
    {}
};


#endif // IMAGECAPTUREINTERFACEQT_H
