#ifndef CALCULATIONSTATS_H_
#define CALCULATIONSTATS_H_
/**
 * \file qtStatisticsCollector.h
 * \brief Add Comment Here
 *
 * \date May 10, 2010
 */

#include <limits>
#include <stdint.h>
#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
#include <QtCore/QString>

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"
#include "core/stats/calculationStats.h"

// TESTOPENSOURCE
//#include "viFlowStatisticsDescriptor.h"

using std::string;
using std::vector;

using namespace corecvs;

class QtStatisticsCollector : public BaseTimeStatisticsCollector
{
public:
    QtStatisticsCollector() : BaseTimeStatisticsCollector()
    {
        // FIXME: GOOPEN
#if 0
        mOrderFilters.push_back(new StringFilter(ViFlowStatisticsDescriptor::TOTAL_TIME));
        mOrderFilters.push_back(new StringFilter(ViFlowStatisticsDescriptor::FRAME_HEIGHT));
        mOrderFilters.push_back(new StringFilter(ViFlowStatisticsDescriptor::FRAME_WIDTH));
        mOrderFilters.push_back(new StringFilter(ViFlowStatisticsDescriptor::IDLE_TIME));
        mOrderFilters.push_back(new StringFilter(ViFlowStatisticsDescriptor::CORRECTON_TIME));
#endif
//        printf("QtStatisticsCollector[%p]::QtStatisticsCollector() Adding filters\n", this);
    }

    ~QtStatisticsCollector()
    {
        for (unsigned i = 0; i < mOrderFilters.size(); i++)
        {
            delete_safe(mOrderFilters[i]);
            mOrderFilters[i] = NULL;
        }
//        printf("QtStatisticsCollector[%p]::~QtStatisticsCollector() Deleted filters\n", this);
    }

    class QtPrinterStream
    {
    public:
        QString collector;

        void printUnitedStat(const string &name, int length, const UnitedStat &stat, int /*lineNum*/)
        {
           double mean = 0.0;
           if (stat.number != 0) {
               mean = stat.sum / stat.number;
           }

           if (stat.type == SingleStat::TIME) {
               collector +=
                   QString("%1 : %2 us : %3 ms : %4 us \n")
                   .arg(name.c_str(), -length)
                   .arg(stat.last, 7)
                   .arg((stat.last + 500) / 1000, 7)
                   .arg(mean, 7);
           } else {
               collector +=
                   QString("%1 : %2 : %4 \n")
                   .arg(name.c_str(), -length)
                   .arg(stat.last, 10)
                   .arg(mean, 10);
           }
        }
    };

    virtual QString printForQT()
    {
        QtPrinterStream stream;
        printStats<QtPrinterStream>(stream);
        return stream.collector;
    }

};

#endif /* CALCULATIONSTATS_H_ */
