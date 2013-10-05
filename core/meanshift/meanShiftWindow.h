#ifndef MEANSHIFTWINDOW_H_
#define MEANSHIFTWINDOW_H_
/**
 * \file meanShiftWindow.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author tbryksin
 * \author ylitvinov
 */

#include "flowBuffer.h"
#include "tileGrid.h"
#include "abstractMeanShiftKernel.h"
#include <vector>
namespace corecvs {

typedef std::vector<std::pair<unsigned, unsigned> > WindowPath;

class MeanShiftWindow {
public:
    MeanShiftWindow(AbstractMeanShiftKernel *kernel, unsigned minX, unsigned minY
            , unsigned maxX, unsigned maxY, unsigned minimalMovement);

    bool moveWindow();

    WindowPath path() const;
    unsigned points() const;
    bool stopped() const;

    int minX() const;
    int minY() const;
    int maxX() const;
    int maxY() const;

private:
    static int mNextId;
    int mId;

    AbstractMeanShiftKernel *mKernel;

    int mMinX;
    int mMinY;
    int mMaxX;
    int mMaxY;

    int mCenterX;
    int mCenterY;

    WindowPath mCenters;

    unsigned mMinimalMovement;

    unsigned mLastPointsCount;
    bool mStopped;
    unsigned mMinDisp;
};


} //namespace corecvs
#endif // MEANSHIFTWINDOW


