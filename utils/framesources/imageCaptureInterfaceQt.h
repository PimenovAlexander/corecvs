#ifndef IMAGECAPTUREINTERFACEQT_H
#define IMAGECAPTUREINTERFACEQT_H

#include <QtCore/QObject>

#include "core/framesources/imageCaptureInterface.h"


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
    void    newCameraParamValue(int idParam);
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
       // SYNC_PRINT(("ImageCaptureInterfaceQt::newFrameReadyCallback(frame_data_t frameData)\n"));
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

    virtual void newCameraParamValueCallBack(int idParam) override
    {
        emit newCameraParamValue(idParam);
    }
};

template<class WrapperType>
class ImageCaptureInterfaceWrapper : public virtual ImageCaptureInterfaceQt, public virtual WrapperType
{
public:
    ImageCaptureInterfaceWrapper(const std::string &input, bool isRGB)
        : WrapperType(input, isRGB)
    {}

    ImageCaptureInterfaceWrapper(const std::string &input, unsigned int mode)
        : WrapperType(input, mode)
    {}

    ImageCaptureInterfaceWrapper(const std::string &input, bool isVerbose, bool isRGB)
        : WrapperType(input, isVerbose, isRGB)
    {}


    ImageCaptureInterfaceWrapper(const std::string &input)
        : WrapperType(input)
    {}

    ImageCaptureInterfaceWrapper(const string &_devname, int h, int w, int fps, bool isRgb)
        : WrapperType(_devname, h, w, fps, isRgb)
    {}

    ImageCaptureInterfaceWrapper(G12Buffer *_cl, G12Buffer *_cr, G12Buffer *_cln, G12Buffer *_crn)
        : WrapperType(_cl, _cr, _cln, _crn)
    {}
};


#endif // IMAGECAPTUREINTERFACEQT_H
