/**
 * \file FileCapture.cpp
 * \brief Add Comment Here
 *
 * \date Apr 9, 2010
 * \author alexander
 */
#include <iostream>
#include <stdio.h>
#include <cstdio>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QtCore>

#include "global.h"

#include "fileCapture.h"
#include "bufferFactory.h"

/* File capture interface */
FileCaptureInterface::FileCaptureInterface(string pathFmt, bool isVerbose)
    : ImageFileCaptureInterface(&pathFmt, isVerbose)
{
    cout << "Starting capture form file with pattern:" << mPathFmt << "\n";
}

FileCaptureInterface::FramePair FileCaptureInterface::getFrame()
{
    FramePair result;

    string name0 = getImageFileName(mCount, 0);
    string name1 = getImageFileName(mCount, 1);

    if (mVerbose) {
        printf("Grabbing frame from file: %s\n", name0.c_str());
    }

    result.bufferLeft = BufferFactory::getInstance()->loadG12Bitmap(name0);

    if (result.bufferLeft)
    {
        if (mVerbose) {
            printf("Grabbing frame from file: %s\n", name1.c_str());
        }

        result.bufferRight = BufferFactory::getInstance()->loadG12Bitmap(name1);
    }

    if (!result.bufferLeft || !result.bufferRight)
    {
        delete result.bufferLeft;
        delete result.bufferRight;

        if (mVerbose) {
            printf("File not found, resetting to first image in the sequence.\n");
        }

        resetImageFileCounter();
        return getFrame();
    }

    increaseImageFileCounter();
    return result;
}

ImageCaptureInterface::CapErrorCode FileCaptureInterface::initCapture()
{
    return ImageCaptureInterface::SUCCESS;
}

ImageCaptureInterface::CapErrorCode FileCaptureInterface::startCapture()
{
    return ImageCaptureInterface::SUCCESS;
}

FileCaptureInterface::~FileCaptureInterface()
{
}

// ****************************************************************************

/** Capture interface with precise simulation from the value "fps" view point
 */
FilePreciseCapture::FilePreciseCapture(QString const &params, bool isVerbose)
    : AbstractFileCapture(params)
    , mCurrentCount(-1)
{
    mVerbose = isVerbose;

    mSpin = new FileSpinThread(this, mDelay, mCurrent);

    cout << "Starting prec: capture form file with pattern:" << mPathFmt << "\n";
}

FilePreciseCapture::FramePair FilePreciseCapture::getFrame()
{
    FramePair result;

    if (mCurrentCount < 0)
    {
        printf("Problem. Contract broken. You should not call getFrame() before you are notified\n");
        return result;
    }

    mProtectFrame.lock();
        string leftFileName = getImageFileName(mCurrentCount, 0);
        if (mVerbose)
            printf("prec: grabbing left  :%s\n", leftFileName.c_str());

        result.bufferLeft    = BufferFactory::getInstance()->loadG12Bitmap(leftFileName.c_str());
        result.leftTimeStamp = mTimeStamp;

        string rightFileName = getImageFileName(mCurrentCount, 1);
        if (mVerbose)
            printf("prec: grabbing right :%s, %d\n", rightFileName.c_str(), mShouldSkipUnclaimed != false);

        result.bufferRight    = BufferFactory::getInstance()->loadG12Bitmap(rightFileName.c_str());
        result.rightTimeStamp = mTimeStamp;

        if (!mShouldSkipUnclaimed)
            increaseImageFileCounter();
    mProtectFrame.unlock();

    return result;
}

FilePreciseCapture::FileSpinThread::FileSpinThread(AbstractFileCapture *pInterface, int delay, ImageCaptureInterface::FramePair &framePair)
    : AbstractFileCaptureSpinThread(pInterface, delay, framePair)
{
}

bool FilePreciseCapture::FileSpinThread::grabFramePair()
{
    FilePreciseCapture *pInterface = static_cast<FilePreciseCapture *>(mInterface);

    if (pInterface->mShouldSkipUnclaimed)
    {
        pInterface->increaseImageFileCounter();
        pInterface->mTimeStamp += pInterface->mDelay * 1000;
    }

    if ((int)pInterface->mCount != pInterface->mCurrentCount)
    {
        string leftFileName  = pInterface->getImageFileName(pInterface->mCount, 0);
        string rightFileName = pInterface->getImageFileName(pInterface->mCount, 1);

        if (QFile::exists(leftFileName.c_str()) &&
            QFile::exists(rightFileName.c_str()))
        {
            pInterface->mCurrentCount = pInterface->mCount;
            pInterface->mTimeStamp   += pInterface->mDelay * 1000;
            if (pInterface->mVerbose)
                printf("prec: Notification about frame %d : [%s|%s]\n",
                    pInterface->mCurrentCount,
                    leftFileName.c_str(),
                    rightFileName.c_str());
        }
        else
        {
            if (pInterface->mVerbose)
                printf("prec: File <%s> or <%s> not found, resetting to first image in the sequence\n",
                    leftFileName .c_str(),
                    rightFileName.c_str());

            // Protection from the stack overflow, when there're no files at the given path
            //
            if (pInterface->mCurrentCount < 0) {
                ASSERT_TRUE(pInterface->mCount == 1, "wrong image counting");
                return false;                               // there's no chance to detect the right path to files
            }
            pInterface->resetImageFileCounter();

            return grabFramePair();
        }
    }

    return true;
}
