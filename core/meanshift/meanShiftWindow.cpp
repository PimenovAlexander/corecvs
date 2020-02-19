/**
 * \file meanShiftWindow.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author tbryksin
 * \author ylitvinov
 */
#include "meanshift/meanShiftWindow.h"

namespace corecvs {

using std::pair;
using std::make_pair;

int const tileSize = 15;

int MeanShiftWindow::mNextId = 0;

MeanShiftWindow::MeanShiftWindow(AbstractMeanShiftKernel *kernel, unsigned minX
     , unsigned minY, unsigned maxX, unsigned maxY, unsigned minimalMovement)
    : mId(mNextId++), mKernel(kernel), mMinX(minX), mMinY(minY)
    , mMaxX(maxX), mMaxY(maxY), mMinimalMovement(minimalMovement)
    , mLastPointsCount(0), mStopped(false)
{
    mCenterX = (mMaxX + mMinX) / 2;
    mCenterY = (mMaxY + mMinY) / 2;
//    qDebug() << "   window " << mId << "created in (" << mCenterX << "," << mCenterY << ")";

    mCenters.push_back(pair<unsigned, unsigned>(mCenterX, mCenterY));
}

bool MeanShiftWindow::moveWindow()
{
    if (!mKernel)
        return false;
//    qDebug() << "trying to move window " << mId;

    unsigned pointsCount = 0;
    unsigned centerX = 0;
    unsigned centerY = 0;
    unsigned kernelSum = 0;

    mKernel->setWindow(mMinX, mMinY, mMaxX, mMaxY);

    for (int x = mMinX - tileSize; x < mMaxX + tileSize; ++x)
    {
        for (int y = mMinY - tileSize; y < mMaxY + tileSize; ++y)
        {
            unsigned const kernelValue = mKernel->value(x, y);

            centerX += x * kernelValue;
            centerY += y * kernelValue;
            kernelSum += kernelValue;
        }
    }

//    qDebug() << "window" << mId << ", kernelSum:" << kernelSum << ", coords: "
//            << mMinX << mMaxX << mMinY << mMaxY;

    if (kernelSum == 0)
    {
        mStopped = true;
        return false;
    }

    centerX = static_cast<unsigned>(centerX / kernelSum);
    centerY = static_cast<unsigned>(centerY / kernelSum);

    int const dx = (centerX - mCenterX);
    int const dy = (centerY - mCenterY);

//    qDebug() << "window" << mId << "has been moved, dx: " << dx << ", dy: " << dy
//            << ", mMinimalMovement: " << mMinimalMovement;

    if ((unsigned)std::abs(dx) <= mMinimalMovement && (unsigned)std::abs(dy) <= mMinimalMovement)
    {
        mStopped = true;
        return false;
    }

    mMinX += dx;
    mMaxX += dx;
    mMinY += dy;
    mMaxY += dy;

    mCenterX = centerX;
    mCenterY = centerY;
//    qDebug() << "   window " << mId << "is being moved to new center: (" << centerX << "," << centerY << ")";

    mCenters.push_back(make_pair(mCenterX, mCenterY));
//    qDebug() << "added center to window " << mId << ". now there're " << mCenters.size() << "of them";
    mLastPointsCount = pointsCount;
    return true;
}

WindowPath MeanShiftWindow::path() const
{
//    qDebug() << "returning " << mCenters.size() << " centers from window " << mId;
    return mCenters;
}

unsigned MeanShiftWindow::points() const
{
    return mLastPointsCount;
}

bool MeanShiftWindow::stopped() const
{
    return mStopped;
}

int MeanShiftWindow::minX() const
{
    return mMinX;
}

int MeanShiftWindow::minY() const
{
    return mMinY;
}

int MeanShiftWindow::maxX() const
{
    return mMaxX;
}

int MeanShiftWindow::maxY() const
{
    return mMaxY;
}


} //namespace corecvs

