/**
 * \file preciseTimer.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */

#ifdef WIN32
#define WINDOWS_CLOCK
#endif

#ifdef QT_CLOCK
#include <QTime>
#endif

#ifdef WINDOWS_CLOCK
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <stdlib.h>

#include "preciseTimer.h"

namespace corecvs {

#ifdef WINDOWS_CLOCK
int64_t PreciseTimer::mFreq = 0L;
#endif

PreciseTimer PreciseTimer::currentTime()
{
    PreciseTimer T;

#if defined(QT_CLOCK)
    QTime time = QTime::currentTime();
    T.setTime(((((int64_t)time.hour() * 60 + time.minute()) * 60 + time.second()) * 1000 + time.msec()) * 1000);
#elif defined(WINDOWS_CLOCK)
    if (!mFreq) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&mFreq);
    }
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);
    T.setTime((time.QuadPart * 1000000) / mFreq);
#else
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);
    T.setTime(timeVal.tv_sec * 1000000 + timeVal.tv_usec);
#endif

    return T;
}

} //namespace corecvs
