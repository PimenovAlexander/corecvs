#pragma once
/**
 * \file preciseTimer.h
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Mar 24, 2010
 * \author alexander
 */
#include <stdint.h>
#include "mathUtils.h"

namespace corecvs {

class PreciseTimer
{
public:
    PreciseTimer()                              { init(); }
    PreciseTimer(int msec) : mTime(msec*1000)   {}

    static  PreciseTimer currentTime();

    void    init()                                  { mTime = -1;                            }
    bool    isCorrect() const                       { return mTime != -1;                    }
    void    setTime(int64_t time)                   { mTime = time;                          }

    int64_t msec(int64_t time) const                { return _roundDivUp(time, 1000);        }

    int64_t usecsTo(PreciseTimer const &end) const  { return end.mTime - this->mTime;        }
    int64_t usecsToNow() const                      { return isCorrect() ? usecsTo(currentTime()) : 0; }
    int64_t usec() const                            { return mTime;                          }
    int64_t msec() const                            { return msec(mTime);                    }

    int operator-(PreciseTimer& other)              { return msec(mTime - other.mTime);      }
    int operator-(int& msecOther)                   { return msec(mTime - msecOther * 1000); }

private:
    int64_t mTime;                                          // keeps the time in mks (usecs)

#ifdef WIN32
    static int64_t mFreq;
#endif
};

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

} //namespace corecvs
