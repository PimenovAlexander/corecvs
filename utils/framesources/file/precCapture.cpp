/**
 * \file precCapture.cpp
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

#include "core/utils/global.h"

#include "precCapture.h"
#include "core/buffers/bufferFactory.h"

/** Capture interface with precise simulation from the value "fps" view point
 */
FilePreciseCapture::FilePreciseCapture(const std::string &params, bool isVerbose, bool isRGB)
    : AbstractFileCapture(params)
    , mCurrentCount(-1)
{
    mIsRgb = isRGB;
    mVerbose = isVerbose;

    mSpin = new FileSpinThread(this, mDelay, mCurrent);

    SYNC_PRINT(( "FilePreciseCapture::FilePreciseCapture(%s, %s, rgb=%s): capture form pattern:%s",
                 params.c_str(),
                 isVerbose ? "true" : "false",
                 isRGB ? "true" : "false",
                 mPathFmt.c_str()));
}

FilePreciseCapture::FramePair FilePreciseCapture::getFrame()
{
    FramePair result;
    mVerbose = true;

    if (mCurrentCount < 0)
    {
        printf("Problem. Contract broken. You should not call getFrame() before you are notified\n");
        return result;
    }

    if (mVerbose)
    {
        printf("prec: Will load (%s)", mIsRgb ? "rgb" : "gray");
    }

    mProtectFrame.lock();
        for (int frameId = 0; frameId < FilePreciseCapture::MAX_INPUTS_NUMBER; frameId++ )
        {
            string fileName = getImageFileName(mCurrentCount, frameId);
            if (mVerbose)
                printf("prec: grabbing frame channel %d  :%s\n", frameId, fileName.c_str());

            if (mIsRgb) {
                result.buffers[frameId].rgbBuffer = BufferFactory::getInstance()->loadRGB24Bitmap(fileName.c_str());

                if (result.buffers[frameId].rgbBuffer != NULL) {
                    result.buffers[frameId].g12Buffer = result.buffers[frameId].rgbBuffer->toG12Buffer();
                }
            } else {
                result.buffers[frameId].g12Buffer = BufferFactory::getInstance()->loadG12Bitmap(fileName.c_str());
            }
            result.buffers[frameId].timeStamp = mTimeStamp;
        }

        SYNC_PRINT(("Loaded ["));
        for (int frameId = 0; frameId < FilePreciseCapture::MAX_INPUTS_NUMBER; frameId++ )
            SYNC_PRINT(("%s", result.buffers[frameId].g12Buffer == NULL ? "-" : "+" ));
        SYNC_PRINT(("]\n"));

        if (!mShouldSkipUnclaimed) {
            increaseImageFileCounter();
        }
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
        string names[ImageCaptureInterface::MAX_INPUTS_NUMBER];

        bool hasFrame = false;
        for (int i = 0; i < ImageCaptureInterface::MAX_INPUTS_NUMBER; i++)
        {
            names[i] = pInterface->getImageFileName(pInterface->mCount, i);
            if (QFile::exists(names[i].c_str()))
            {
                hasFrame = true;
            }
        }

        if (hasFrame)
        {
            pInterface->mCurrentCount = pInterface->mCount;
            pInterface->mTimeStamp   += pInterface->mDelay * 1000;
            if (pInterface->mVerbose)
                printf("prec: Notification about frame %d : [%s|%s]\n",
                    pInterface->mCurrentCount,
                    names[ImageCaptureInterface::LEFT_FRAME ].c_str(),
                    names[ImageCaptureInterface::RIGHT_FRAME].c_str());
        }
        else
        {
            if (pInterface->mVerbose)
                printf("prec: Files for cout not found, resetting to first image in the sequence\n");

            // Protection from the stack overflow, when there're no files at the given path
            //
            if (pInterface->mCurrentCount < 0) {
                CORE_ASSERT_TRUE(pInterface->mCount == 1, "wrong image counting");
                return false;                               // there's no chance to detect the right path to files
            }
            pInterface->resetImageFileCounter();

            return grabFramePair();
        }
    }

    return true;
}
