/**
 * \file Timeliner.h
 * \brief Add Comment Here
 *
 * \date Aug 29, 2011
 * \author alexander
 */

#ifndef TIMELINER_H_
#define TIMELINER_H_

#include <limits>
#include <stdint.h>
#include <QtCore/QString>

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"

using namespace corecvs;

#if 0

template<class StatisticsDescriptor>
class BaseTimeStatistics : public StatisticsDescriptor
{
public:
    static const unsigned SIZE = StatisticsDescriptor::MAX_ID;

    FixedVector<uint64_t, SIZE> times;
    static const char **getNames()
    {
        return StatisticsDescriptor::names;
    }

    BaseTimeStatistics()
    {
       times = FixedVector<uint64_t, SIZE>((uint64_t)0);
    }
};



template<class Statistics>
class BaseTimeStatisticsCollector
{
public:
    int added;
    FixedVector<uint64_t, Statistics::SIZE> current;
    FixedVector<uint64_t, Statistics::SIZE> mean;
    FixedVector<uint64_t, Statistics::SIZE> squareMean;
    FixedVector<uint64_t, Statistics::SIZE> min;
    FixedVector<uint64_t, Statistics::SIZE> max;

    BaseTimeStatisticsCollector() :
        added(0),
        current((uint64_t)0),
        mean((uint64_t)0),
        squareMean((uint64_t)0),
        min(std::numeric_limits<uint64_t>::max()),
        max((uint64_t)0)
    {

    }

    virtual void addStatistics(Statistics &stats)
    {
         for (unsigned i = 0; i < Statistics::SIZE; i++)
        {
            uint64_t value = stats.times[i];
            current[i]     = value;
            mean[i]       += value;
            squareMean[i] += value * value;
            if ((min[i] > value) && (value != 0)) min[i] = value;
            if (max[i] < value) max[i] = value;
        }
        added ++;
    }

    virtual void printSimple()
    {
        printf("=============================================================\n");
        for (unsigned i = 0; i < Statistics::SIZE; i++)
        {
            printf("%-20s : %7" PRIu64 " us : %7" PRIu64 " ms  \n",
                    Statistics::getNames()[i],
                    mean[i] / added,
                    ((mean[i] / added) + 500) / 1000);
        }
        printf("=============================================================\n");
    }

    virtual void printAdvanced()
    {
        printf("=============================================================\n");
        printf("%-20s : %-10s : %-10s : %-10s \n", "Stage", "Avg us", "Avg ms", "Min us ");
        for (unsigned i = 0; i < Statistics::SIZE; i++)
        {
            printf("%-20s : %7" PRIu64 " us : %7" PRIu64 " ms : %7" PRIu64 " us  \n",
                    Statistics::getNames()[i],
                    mean[i] / added,
                    ((mean[i] / added) + 500) / 1000,
                    min[i]);
        }
        printf("=============================================================\n");
    }

    virtual QString printForQT()
    {
        QString stats;
        for (unsigned i = 0; i < Statistics::SIZE; i++)
        {
            stats += QString("%1 : %2 us : %3 ms : %4 us \n")
                     .arg(Statistics::getNames()[i], -20)
                     .arg(current[i], 7)
                     .arg((current[i] + 500) / 1000, 7)
                     .arg(mean[i] / added, 7);
        }
        return stats;
    }


};
#endif

/**
 *   This is a singleton class that collects the periodic events and measure delays
 **/
class Timeliner
{
public:
    Timeliner();
    enum {
        START_OF_LOOP,
        END_OF_COMPUTATION,
        END_OF_DRAW,

        END_OF_LOOP,
        MAX_ID
    };

    virtual ~Timeliner();

};

#endif /* TIMELINER_H_ */
