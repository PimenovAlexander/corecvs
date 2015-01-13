/**
 * \file calculationStats.h
 * \brief Add Comment Here
 *
 * \date May 10, 2010
 * \author alexander
 */

#ifndef CORECALCULATIONSTATS_H_
#define CORECALCULATIONSTATS_H_

#include <limits>
#include <stdint.h>

#include <map>
#include <string>

#include "global.h"

#include "string.h"
#include "fixedVector.h"
#include "preciseTimer.h"
#include "calculationStats.h"

namespace corecvs {

using std::map;
using std::string;

/**
 *  Single statistics entry
 **/
struct SingleStat {
    enum Type {
        UNKNOWN,
        TIME,
        UNSIGNED,
        SIGNED,
        DOUBLE
    };

    Type type;
    uint64_t value;

    SingleStat() :
        type(UNKNOWN),
        value(0)
    {}

    SingleStat(uint64_t _value, Type _type = TIME) :
        type(_type),
        value(_value)
    {}
};

struct UnitedStat {

    SingleStat::Type type;

    uint64_t last;
    uint64_t sum;
    uint64_t min;
    uint64_t max;
    uint64_t number;
    uint64_t sumSq;

    UnitedStat()
    {
        memset(this, 0, sizeof(UnitedStat));
        type = SingleStat::UNKNOWN;
    }

    void addValue(uint64_t value)
    {
       number++;
       last = value;
       sum += value;
       sumSq += value * value;

       if ((min > value) && (value != 0)) {
           min = value;
       }
       if (max < value) {
           max = value;
       }
    }


    void addValue(const SingleStat &stat)
    {
       type = stat.type;
       addValue(stat.value);
    }
};


class Statistics
{
public:
    /* New interface */
    map<string, SingleStat> values;
    string prefix;
    PreciseTimer helperTimer;

    void startInterval()
    {
        helperTimer = PreciseTimer::currentTime();
    }

    void endInterval(const string &str)
    {
        values[prefix + str] = SingleStat(helperTimer.usecsToNow());
    }

    void resetInterval(const string &str)
    {
        endInterval(str);
        startInterval();
    }

    void setTime(const string &str, uint64_t delay)
    {
        values[prefix + str] = SingleStat(delay);
    }

    void setValue(const string &str, uint64_t value)
    {
        values[prefix + str] = SingleStat(value, SingleStat::UNSIGNED);
    }
};


class BaseTimeStatisticsCollector
{
public:
    typedef map<string, UnitedStat> StatsMap;

    StatsMap sumValues;

    /* Class for filtering */
    class OrderFilter
    {
    public:
        virtual bool filter(const string &/*input*/)
        {
            return false;
        }
        virtual ~OrderFilter() {};
    };

    class StringFilter : public OrderFilter {
    public:
        string mFilterString;

        StringFilter(string filterString) :
            mFilterString(filterString)
        {}

        virtual bool filter(const string &input)
        {
//            printf("%s =? %s\n", input.c_str(), mFilterString.c_str());
            return (input == mFilterString);
        }

    };

    // TODO: it's unsafe to copy this
    vector<OrderFilter *> mOrderFilters;

    BaseTimeStatisticsCollector()
    {

    }

    virtual void reset()
    {
        sumValues.clear();
    }

    virtual void addSingleStat(string name, const SingleStat &stat)
    {
        UnitedStat unitedStat;
        map<string, UnitedStat>::iterator uit;
        uit = sumValues.find(name);
        if (uit != sumValues.end())
        {
           unitedStat = uit->second;
        }

        unitedStat.addValue(stat);
        sumValues[name] = unitedStat;
    }


    virtual void addStatistics(Statistics &stats)
    {
        /* New style */
        map<string, SingleStat>::iterator it;
        for (it = stats.values.begin(); it != stats.values.end(); ++it )
        {
            string name = it->first;
            SingleStat stat = it->second;
            addSingleStat(name, stat);
        }
    }

    virtual int maximumCaptionLength() const
    {
        StatsMap::const_iterator uit;
        int maxCaptionLen = 0;
        for (uit = this->sumValues.begin(); uit != this->sumValues.end(); ++uit )
        {
            int len = (int)uit->first.length();
            if (len > maxCaptionLen)
            {
                maxCaptionLen = len;
            }
        }
        return maxCaptionLen;
    }


template <class StreamType>
    void printStats(StreamType &stream) const
    {
        typename StatsMap::const_iterator uit;
        /* Get caption width */
        int maxCaptionLen = maximumCaptionLength();

        /* Flags */
        int size = (int)this->sumValues.size();
        bool *isPrinted = new bool[size];
        for (int i = 0; i < size; i++)
        {
            isPrinted[i] = false;
        }

        int printCount = 0;
        /* First show all filters */
        //printf("Totaly we have %d filters\n", mOrderFilters.size());
        for (unsigned i = 0; i < mOrderFilters.size(); i++)
        {
            OrderFilter *filter = mOrderFilters[i];
            int j = 0;
            for (uit = this->sumValues.begin(); uit != this->sumValues.end(); ++uit, j++ )
            {
                if (isPrinted[j])
                {
                    continue;
                }

                if (filter->filter(uit->first))
                {
                    isPrinted[j] = true;
                    stream.printUnitedStat(uit->first, maxCaptionLen, uit->second, printCount);
                    printCount++;
                }
            }
        }

        int j = 0;
        for (uit = this->sumValues.begin(); uit != this->sumValues.end(); ++uit, j++ )
        {
            if (isPrinted[j])
            {
                continue;
            }

            stream.printUnitedStat(uit->first, maxCaptionLen, uit->second, printCount);
            printCount++;
        }
        deletearr_safe(isPrinted);
    }

    class SimplePrinter {
    public:
        void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
        {
            if (stat.type == SingleStat::TIME)
            {
                printf("%-20s : %7" PRIu64 " us : %7" PRIu64 " ms  \n",
                    name.c_str(),
                    stat.sum / stat.number,
                    ((stat.sum / stat.number) + 500) / 1000);
                return;
            }
            printf("%-20s : %7" PRIu64 "\n",
                name.c_str(),
                stat.sum / stat.number);

         }
    };

    virtual void printSimple()
    {
        printf("=============================================================\n");
        SimplePrinter printer;
        printStats(printer);
        printf("=============================================================\n");
    }

    class AdvancedPrinter {
    public:
        void printUnitedStat(const string &name, int length, const UnitedStat &stat, int /*lineNum*/)
        {
            if (stat.type == SingleStat::TIME)
            {
                printf("%-*s : %7" PRIu64 " us : %7" PRIu64 " ms : %7" PRIu64 " us  \n",
                    length,
                    name.c_str(),
                    stat.sum / stat.number,
                    ((stat.sum / stat.number) + 500) / 1000,
                    (stat.min == std::numeric_limits<uint64_t>::max()) ? 0 : stat.min);
                return;
            }

            printf("%-*s : %7" PRIu64 "\n",
                length,
                name.c_str(),
                stat.sum / stat.number);

         }
    };

    virtual void printAdvanced()
    {
        printf("=============================================================\n");
        AdvancedPrinter printer;
        printStats(printer);
        printf("=============================================================\n");
    }


    class WikiPrinter {
    public:
        void printUnitedStat(const string &/*name*/, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
        {
            printf("%7" PRIu64 "|", stat.sum / stat.number);
        }
    };

    virtual void printWiki()
    {
        printf("|");
/*        for (unsigned i = 0; i < Statistics::SIZE; i++)
        {
            printf("%s|", Statistics::getNames()[i]);
        }*/
        printf("\n");
        printf("====================Paste this to  wiki======================\n");

        const char *withsse = "";
#ifdef WITH_SSE
        withsse = "!y.png!";
#endif

        const char *withtbb = "";
#ifdef WITH_TBB
        withtbb = "!y.png!";
#endif

        const char *osId = "!l.png!";
#ifdef Q_OS_WIN
        osId = "!w.png!";
#endif
#ifdef Q_OS_MAC
        osId = "!a.png!"
#endif

        const char *is64bit = "!3.png!";
#ifdef __x86_64
    is64bit = "!6.png!";
#endif

        const char *compiler = "unkn";
#if defined (__GNUC__) && defined (__GNUC_MINOR__)
#define GCC_XSTR(value) GCC_STR(value)
#define GCC_STR(value) #value
        compiler = "g" GCC_XSTR(__GNUC__) "." GCC_XSTR(__GNUC_MINOR__);
#undef GCC_STR
#undef GCC_XSTR
#elif defined (__clang__)
        compiler = "clan";
#elif defined (_MSC_VER)
        compiler = "msvc";
#elif defined (__MINGW32__)
        compiler = "mgw";
#elif defined (__INTEL_COMPILER)
        compiler = "icc";
#endif

               //       OS  bits comp tbb  sse
        printf("|%-15s|%-7s|%-7s|%-4s|%-7s|%-7s|", " ", osId, is64bit, compiler, withtbb, withsse);
        WikiPrinter printer;
        printStats(printer);
        printf("\n");
        printf("=============================================================\n");
    }
};


} //namespace corecvs
#endif /* CORECALCULATIONSTATS_H_ */

