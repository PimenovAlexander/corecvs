/**
 * \file imagerControl.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 2, 2010
 * \author alexander
 */

#include "imagerControl.h"
namespace corecvs {

#define TRACE_CONTROL_DETAILS

// Could be dangerous
#ifdef TRACE_CONTROL_DETAILS
#define TRACE_CONTROL  printf
#else
#define TRACE_CONTROL
#endif


double ImagerControl::gainScaleDb[] =
{
    /// Set gain to 0 db.         0000
    0.0,
    /// Set gain to 0.56 db.     0001
    0.56,
    /// Set gain to 1.16 db.     0010
    1.16,
    /// Set gain to 1.8 db.     0011
    1.8,
    /// Set gain to 2.5 db.     0100
    2.5,
    /// Set gain to 3.25 db.     0101
    3.25,
    /// Set gain to 4.1 db.     0110
    4.1,
    /// Set gain to 5.0 db.     0111
    5.0,
    /// Set gain to 6.0 db.     1000
    6.0,
    /// Set gain to 7.2 db.     1001
    7.2,
    /// Set gain to 8.5 db.     1010
    8.5,
    /// Set gain to 10.1 db.     1011
    10.1,
    /// Set gain to 12 db.         1100
    12.0,
    /// Set gain to 14.5 db.     1101
    14.5,
    /// Set gain to 18.1 db.     1110
    18.1,
    /// Set gain to 24.1 db.     1111
    24.1
};

#define GAIN_LEVEL_NUMBER   CORE_COUNT_OF(ImagerControl::gainScaleDb)

double   ImagerControl::gainAbsolute[GAIN_LEVEL_NUMBER];
unsigned ImagerControl::gainLevelNumber = ImagerControl::transformToAbsolute();

double ImagerControl::getMultipierForDb (double db)
{
    return pow(10, 0.1 * db);
}

unsigned ImagerControl::transformToAbsolute()
{
    for (unsigned i = 0; i < GAIN_LEVEL_NUMBER; i++)
    {
        gainAbsolute[i] = getMultipierForDb(ImagerControl::gainScaleDb[i]);
    }
    return GAIN_LEVEL_NUMBER;
}


void ImagerControl::input(ContolInput &input)
{
    nextState = currentState;
    int gain = currentState.gain;
    double overexpo  = input.histPtr->getOverExpo();
    double underexpo = input.histPtr->getUnderExpo();

    TRACE_CONTROL("CONTROL: Current gain is %u\n", gain);
    TRACE_CONTROL("CONTROL: Over expo %2.2lf\n", overexpo * 100.0 );
    TRACE_CONTROL("CONTROL: Under expo %2.2lf\n", underexpo * 100.0 );

    if (overexpo > persentile)
    {
        nextState.gain = gain;
        TRACE_CONTROL("CONTROL: Decided to increase gain %d+1\n", gain);
        return;
    }

    if (gain == 0)
    {
        return;
    }

    /* Now we should check if we should not reduce the gain */
    int highPersentle = input.histPtr->getHigherPersentile(persentile);
    TRACE_CONTROL("CONTROL: The level of high percentile %u of %u\n", highPersentle, 0xFFF);

    // TODO: ASAP get rid of the constant should somehow reuse the
    double ratio = (double)(0xFFF - highPersentle) / 0xFFF;
    double predictedRatio = 1.0;

    do {
        gain--;
        predictedRatio =  gainAbsolute[gain] / gainAbsolute[currentState.gain];
        TRACE_CONTROL("CONTROL: Decided to decrease gain %d-1\n", gain);
    } while (predictedRatio > ratio && gain > 0);

    nextState.gain = gain;
}

ImagerState ImagerControl::reaction()
{
    currentState = nextState;
    return nextState;
}


} //namespace corecvs

