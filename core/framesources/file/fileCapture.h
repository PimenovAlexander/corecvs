#pragma once
/**
 * \file fileCapture.h
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */

#include "framesources/file/imageFileCaptureInterface.h"
#include "abstractFileCaptureSpinThread.h"
#include "abstractFileCapture.h"

/**
 * This class implements the capture from a list of files.
 * It sends newFrameReady signal immediately. More precise camera simulation
 * is done in FilePreciseCaptureInterface class
 * */
class FileCaptureInterface : public virtual ImageFileCaptureInterface
{
public:
    FileCaptureInterface(string pathFmt, bool isVerbose = true, bool isRGB = false);

    virtual FramePair    getFrame();

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();
    virtual CapErrorCode nextFrame();

    virtual ~FileCaptureInterface();
};

class FileCaptureProducer : public ImageCaptureInterfaceProducer
{
public:
    FileCaptureProducer()
    {}

    virtual std::string getPrefix() override
    {
        return "file:";
    }

    virtual ImageCaptureInterface *produce(std::string &name, bool isRGB) override
    {
        return new FileCaptureInterface(name, false, isRGB);
    }
};

