#pragma once
/**
 * \file preciseTimer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 *
 *  This class is a wrapper over a set of time measure functions, typically of usec or better resolution
 *
 *   Common implemetations are
 *    * std::chrono based
 *    * win32 based (QueryPerformanceCounter)
 *    * gettimeofday on linux
 */
#include <stdint.h>
#include "core/math/mathUtils.h"

namespace corecvs {

class PreciseTimer
{
public:
    PreciseTimer()                                  { mTime = -1; }

    /**
     *   We should and want to use CurrentTime name.
     *   Unfortunately X11 has an evil define of the same name in X.h
     *
     **/
    static  PreciseTimer CurrentETime();

    /**
     *  \deprecated
     **/
    static  PreciseTimer currentTime()              { return CurrentETime(); }

    bool    isCorrect() const                       { return mTime != -1;    }

    void    setTime(int64_t time)                   { mTime = time;          }

    int64_t usec() const                            { return mTime;          }
    int64_t msec() const                            { return msec(mTime);    }

    static int64_t msec(int64_t time)               { return _roundDivUp(time, 1000 * 1000);           }

    int64_t msecsTo(PreciseTimer const &end) const  { return _roundDivUp(nsecsTo(end), 1000 * 1000);   }
    int64_t usecsTo(PreciseTimer const &end) const  { return _roundDivUp(nsecsTo(end), 1000);          }
    int64_t nsecsTo(PreciseTimer const &end) const  { return end.mTime - this->mTime;                  }

    int64_t usecsToNow() const                      { return isCorrect() ? usecsTo(currentTime()) : 0; }
    int64_t nsecsToNow() const                      { return isCorrect() ? nsecsTo(currentTime()) : 0; }
    int64_t msecsToNow() const                      { return isCorrect() ? msecsTo(currentTime()) : 0; }

    int     operator-(PreciseTimer& other) const    { return (int)msec(mTime - other.mTime);           }

private:
    /**
     *  keeps the time in nanoseconds (nsecs)
     *  Around 150 years will fit in 63 bits.
     *  2^62  / 10^9 / (60*60*24) / 365 ~=~ 146.2 years
     **/
    int64_t mTime;

#ifdef WIN32
    static double mFreq;
#endif
};


#if 0
class PreciseTimerEx : public PreciseTimer
{
public:
    /** ctor: stores the current time */
    PreciseTimerEx(uint64_t *pAccTime = NULL)
        : mpAccTime(pAccTime)                   { setTime(currentTime().usec());          }

    /** return the elapsed time since the object exists */
    int64_t elapsed() const                     { return usecsToNow();                    }

    /** dtor: accumulates the passed time into accumulator */
    ~PreciseTimerEx()                           { if (mpAccTime) *mpAccTime += elapsed(); }

private:
    uint64_t *mpAccTime;
};
#endif

} //namespace corecvs
