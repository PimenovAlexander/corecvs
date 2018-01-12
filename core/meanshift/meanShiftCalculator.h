#ifndef MEANSHIFTCALCULATOR_H_
#define MEANSHIFTCALCULATOR_H_
/**
 * \file meanShiftCalculator.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Nov 12, 2010
 * \author tbryksin
 * \author ylitvinov
 */

#include "core/meanshift/meanShiftWindow.h"
#include "core/meanshift/abstractMeanShiftKernel.h"

#include <map>
namespace corecvs {

class MeanShiftCalculator {
public:
    MeanShiftCalculator(
            AbstractMeanShiftKernel * const kernel,
            unsigned minX, unsigned minY,
            unsigned maxX, unsigned maxY);

    ~MeanShiftCalculator();

    void calculate(unsigned numberOfWindows, unsigned overlay, unsigned minimalMovement
            , unsigned iterations, unsigned minDisp);

    std::vector<MeanShiftWindow*> windows() const;

private:

    void initWindows(unsigned minimalMovement);

    AbstractMeanShiftKernel *mKernel;  // no ownership
    unsigned const mMinX;
    unsigned const mMinY;
    unsigned const mMaxX;
    unsigned const mMaxY;

    unsigned mWindowsNumber;
    unsigned mOverlay;

    std::map<MeanShiftWindow*, bool> mWindows;
};


} //namespace corecvs
#endif //#ifndef MEANSHIFTCALCULATOR_H_

