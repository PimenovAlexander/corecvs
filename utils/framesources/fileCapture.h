#pragma once
/**
 * \file FileCapture.h
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */
#include <QtCore/QThread>
#include <QtCore/QMutex>

#include "imageFileCaptureInterface.h"
#include "abstractFileCaptureSpinThread.h"
#include "abstractFileCapture.h"

/**
 * This class implements the capture from a list of files.
 * It sends newFrameReady signal immediately. More precise camera simulation
 * is done in FilePreciseCaptureInterface class
 * */
class FileCaptureInterface : public ImageFileCaptureInterface
{
public:
    FileCaptureInterface(string pathFmt, bool isVerbose = true);

    virtual FramePair    getFrame();

    virtual CapErrorCode initCapture();
    virtual CapErrorCode startCapture();
//  virtual void freeFrame(G12Buffer* buffer);

    virtual ~FileCaptureInterface();
};

/***************************************************************************************/

class FilePreciseCapture : public AbstractFileCapture
{
public:
    FilePreciseCapture(QString const &params, bool isVerbose = true);

    virtual FramePair getFrame();

    void increaseCurrentCount() { mCurrentCount++; }

private:

    class FileSpinThread : public AbstractFileCaptureSpinThread
    {
    public:
        FileSpinThread(AbstractFileCapture *pInterface, int delay, ImageCaptureInterface::FramePair &framePair);
        virtual bool grabFramePair();
    };

    int mCurrentCount;
};
