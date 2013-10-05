#ifndef IMAGERSTATE_H_
#define IMAGERSTATE_H_
/**
 * \file imagerControl.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 2, 2010
 * \author alexander
 */

#include <math.h>
#include "histogram.h"
namespace corecvs {

class ImagerState
{
public:
    int gain;
};

class ContolInput
{
public:
    Histogram *histPtr;
};

class ImagerControl
{
protected:
    static double gainScaleDb[];
    static double gainAbsolute[];

    static unsigned gainLevelNumber;
    static unsigned transformToAbsolute();

    ImagerState currentState;
    ImagerState nextState;
    double persentile;
    int lastchange;
public:
    static double getMultipierForDb (double db);

    void setPersentile(double _persentile)
    {
        this->persentile = _persentile;
    }

    void setCurrentState(ImagerState &state)
    {
        this->currentState = state;
    }

    void input(ContolInput &input);
    ImagerState reaction();
};


} //namespace corecvs
#endif /* IMAGERSTATE_H_ */

