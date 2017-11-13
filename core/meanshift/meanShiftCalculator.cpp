/**
 * \file meanShiftCalculator.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author tbryksin
 * \author ylitvinov
 */

#include "core/meanshift/meanShiftCalculator.h"

namespace corecvs {

using std::map;

MeanShiftCalculator::MeanShiftCalculator(AbstractMeanShiftKernel *kernel, unsigned minX, unsigned minY, unsigned maxX, unsigned maxY)
    : mKernel(kernel), mMinX(minX), mMinY(minY), mMaxX(maxX), mMaxY(maxY)
{
}

MeanShiftCalculator::~MeanShiftCalculator()
{
    for (map<MeanShiftWindow*, bool>::iterator it = mWindows.begin(); it != mWindows.end(); it++)
        delete (*it).first;
}

void MeanShiftCalculator::initWindows(unsigned minimalMovement)
{
    unsigned areaWidth = mMaxX - mMinX;
    unsigned areaHeight = mMaxY - mMinY;
    unsigned windowWidth = areaWidth / mWindowsNumber;
    unsigned windowHeight = areaHeight / mWindowsNumber;

    int x = mMinX;
    int y = mMinY;

//    qDebug() << "creating " << mWindowsNumber * mWindowsNumber << " windows";
    for (unsigned j = 0; j < mWindowsNumber; ++j)
    {
        for (unsigned i = 0; i < mWindowsNumber; ++i)
        {
            int windowMinX = (j == 0) ? x : (int)(x - mOverlay);
            int windowMinY = (i == 0) ? y : (int)(y - mOverlay);
            int windowMaxX = (j == mWindowsNumber - 1) ? mMaxX : x + windowWidth + mOverlay;
            int windowMaxY = (i == mWindowsNumber - 1) ? mMaxY : y + windowHeight + mOverlay;

//            qDebug() << "creating window" << j * mWindowsNumber + i << ": " << windowMinX
//                    << windowMaxX << windowMinY << windowMaxY;
            MeanShiftWindow *window =
                    new MeanShiftWindow(mKernel, windowMinX, windowMinY, windowMaxX
                    , windowMaxY, minimalMovement);
            mWindows[window] = true;
            y += windowHeight;
        }
        x += windowWidth;
        y = mMinY;
    }
}

void MeanShiftCalculator::calculate(unsigned numberOfWindows, unsigned overlay
        , unsigned minimalMovement, unsigned iterations, unsigned /* minDisp*/ )
{
    mWindowsNumber = numberOfWindows;
    mOverlay = overlay;

    initWindows(minimalMovement);

    int windowsMoving = 0;
   do {
        iterations--;
        windowsMoving = 0;
        for (map<MeanShiftWindow*, bool>::iterator it = mWindows.begin(); it != mWindows.end(); it++) {
            if (!(*it).second)
                continue;
            windowsMoving++;
            (*it).second = (*it).first->moveWindow();
        }
    } while (windowsMoving > 0 && iterations > 0);
//    for (map<MeanShiftWindow*, bool>::iterator it = mWindows.begin(); it != mWindows.end(); it++) {
//        (*it).first->printPath();
//    }
}

std::vector<MeanShiftWindow*> MeanShiftCalculator::windows() const
{
    std::vector<MeanShiftWindow*> result;
    for (map<MeanShiftWindow*, bool>::const_iterator it = mWindows.begin(); it != mWindows.end(); it++)
        result.push_back((*it).first);
    return result;
}

} //namespace corecvs

