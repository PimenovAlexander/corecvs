#ifndef CORECALCULATIONSTATS_H_
#define CORECALCULATIONSTATS_H_

/**
 * \file calculationStats.h
 * \brief Add Comment Here
 *
 */

#include <limits>
#include <stdint.h>
#include <string.h>

#include <map>
#include <string>
#include <deque>

#include "core/utils/global.h"

#include "core/math/vector/fixedVector.h"
#include "core/utils/preciseTimer.h"
#include "core/math/mathUtils.h"

namespace corecvs {

//using std::map;
using std::string;
using std::deque;

/**
 *  Single statistics entry
 **/
struct SingleStat
{
    enum Type {
        UNKNOWN,
        TIME,
        UNSIGNED,
        SIGNED,
        DOUBLE
    };

    Type type;
    uint64_t value;

    SingleStat() : type(UNKNOWN), value(0)
    {}

    SingleStat(uint64_t _value, Type _type = TIME) : type(_type), value(_value)
    {}
};

struct UnitedStat
{
    SingleStat::Type type;

    uint64_t sum;
    uint64_t sumSq;
    uint64_t last;
    uint64_t min;
    uint64_t max;
    uint64_t number;

    UnitedStat()
    {
        CORE_CLEAR_STRUCT(*this);
        type = SingleStat::UNKNOWN;
    }

    void addValue(uint64_t value)
    {
       sum   += value;
       sumSq += value * value;
       last   = value;
       if (min > value || number == 0) {
           min = value;
       }
       if (max < value) {
           max = value;
       }
       ++number;
    }

    void addValue(const SingleStat &stat)
    {
       type = stat.type;
       addValue(stat.value);
    }

    void unite(const UnitedStat &ustat)
    {
        if (type == SingleStat::UNKNOWN) { type = ustat.type; }
        if (number == 0) { min = ustat.min; max = ustat.max; last = ustat.last; }
        number += ustat.number;
        sum    += ustat.sum;
        sumSq  += ustat.sumSq;
    }

    uint64_t mean()     const { return number ? _roundDivUp(sum, number) : 0; }
    uint64_t minimum()  const { return min; }
};


class Statistics
{
public:
    struct State
    {
        State(const string& prefix, PreciseTimer helperTimer)
            : mPrefix(prefix)
            , mHelperTimer(helperTimer)
        {}

        string       mPrefix;
        PreciseTimer mHelperTimer;
    };

    std::deque<State> mStack;


    /**
     * Checking this for NULL is ABSOLUTLY discouraged! This violates the standard and should be avoided
     *
     * \attention Any call on a NULL pointer is undefined behavior!
     *
     * Use static wrappers instead.
     *
     **/
    Statistics* enterContext(const string& prefix)
    {
        /*if (this == nullptr)
            return nullptr;*/
        mStack.emplace_back(State(mPrefix, mHelperTimer));
        mPrefix = mPrefix + prefix;
        return this;
    }

    void leaveContext()
    {
        /*if (this == nullptr)
            return;*/
        State state = mStack.back();
        mStack.pop_back();
        mPrefix      = state.mPrefix;
        mHelperTimer = state.mHelperTimer;
    }

    /* New interface */
    std::map<string, SingleStat> mValues;
    string                  mPrefix;
    PreciseTimer            mHelperTimer;


    void startInterval()
    {
        /*if (this == nullptr)
            return;*/
        mHelperTimer = PreciseTimer::currentTime();
    }

    void endInterval(const string &str)
    {
        /*if (this == nullptr)
            return;*/
        setTime(str, mHelperTimer.usecsToNow());
    }

    void resetInterval(const string &str)
    {        
        endInterval(str);
        startInterval();
    }

    void setTime(const string &str, uint64_t delay)
    {
        /*if (this == nullptr)
            return;*/
        mValues[mPrefix + str] = SingleStat(delay);
    }

    void setValue(const string &str, uint64_t value)
    {
        /*if (this == nullptr)
            return;*/
        mValues[mPrefix + str] = SingleStat(value, SingleStat::UNSIGNED);
    }

    /**
     * Static calls that check for NULL
     **/

    static Statistics* enterContext(Statistics *stats, const string& prefix)
    {
        if (stats == NULL)
            return NULL;
        return stats->enterContext(prefix);
    }

    static void leaveContext(Statistics *stats)
    {
        if (stats == NULL)
            return;
        stats->leaveContext();
    }

    static void startInterval(Statistics *stats)
    {
        if (stats == nullptr)
            return;
        stats->startInterval();
    }

    static void endInterval(Statistics *stats, const string &str)
    {
        if (stats == nullptr)
            return;
        stats->endInterval(str);
    }

    static void resetInterval(Statistics *stats, const string &str)
    {
        endInterval(stats, str);
        startInterval(stats);
    }

    static void setTime(Statistics *stats, const string &str, uint64_t delay)
    {
        if (stats == nullptr)
            return;
        stats->setTime(str, delay);
    }

    static void setValue(Statistics *stats, const string &str, uint64_t value)
    {
        if (stats == nullptr)
            return;
        stats->setValue(str, value);
    }


    friend ostream & operator <<(ostream &out, const Statistics &stats)
    {
        for(auto &stat : stats.mValues)
        {
            out << stat.first << " -> " << stat.second.value << "\n";
        }
        return out;
    }

};

class BaseTimeStatisticsCollector
{
public:
    /** Types **/
    typedef std::map<string, UnitedStat> StatsMap;

    /* Class for filtering */
    class OrderFilter
    {
    public:
        virtual bool filter(const string &/*input*/) const
        {
            return false;
        }
        virtual ~OrderFilter() {}
    };

    class StringFilter : public OrderFilter
    {
        string mFilterString;
    public:
        StringFilter(const string & filterString) : mFilterString(filterString)
        {}

        virtual bool filter(const string &input) const
        {
//            printf("%s =? %s\n", input.c_str(), mFilterString.c_str());
            return (input == mFilterString);
        }
    };

    StatsMap              mSumValues;

    // TODO: it's unsafe to copy this
    vector<OrderFilter *> mOrderFilters;

    BaseTimeStatisticsCollector() {}
    BaseTimeStatisticsCollector(const Statistics &stats)
    {
        addStatistics(stats);
    }

    virtual void reset()
    {
        mSumValues.clear();
    }

    virtual void addSingleStat(const string& name, const SingleStat &stat)
    {
        UnitedStat unitedStat;
        std::map<string, UnitedStat>::iterator uit = mSumValues.find(name);
        if (uit != mSumValues.end())
        {
           unitedStat = uit->second;
        }
        unitedStat.addValue(stat);
        mSumValues[name] = unitedStat;
    }

    virtual void addStatistics(const Statistics &stats)
    {
        /* New style */
        std::map<string, SingleStat>::const_iterator it;
        for (it = stats.mValues.begin(); it != stats.mValues.end(); ++it)
        {
            string name = it->first;
            SingleStat stat = it->second;
            addSingleStat(name, stat);
        }
    }

    virtual void uniteStatistics(const BaseTimeStatisticsCollector &stats)
    {
        std::map<string, UnitedStat>::const_iterator uit;
        for (uit = stats.mSumValues.begin(); uit != stats.mSumValues.end(); ++uit)
        {
            string     name  = uit->first;
            UnitedStat ustat = uit->second;

            UnitedStat unitedStat;
            std::map<string, UnitedStat>::iterator uit2 = mSumValues.find(name);
            if (uit2 != mSumValues.end())
            {
                unitedStat = uit2->second;
            }
            unitedStat.unite(ustat);
            mSumValues[name] = unitedStat;
        }
    }

    virtual int maximumCaptionLength() const
    {
        StatsMap::const_iterator uit;
        int maxCaptionLen = 0;
        for (uit = mSumValues.begin(); uit != mSumValues.end(); ++uit)
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
        vector<bool> isPrinted(mSumValues.size(), false);

        int printCount = 0;
        /* First show all filters */
        //printf("Totaly we have %d filters\n", mOrderFilters.size());
        for (const OrderFilter *filter : mOrderFilters)
        {
            int j = 0;
            for (uit = mSumValues.begin(); uit != mSumValues.end(); ++uit, j++)
            {
                if (isPrinted[j])
                    continue;

                if (filter->filter(uit->first))
                {
                    isPrinted[j] = true;
                    stream.printUnitedStat(uit->first, maxCaptionLen, uit->second, printCount);
                    printCount++;
                }
            }
        }

        int j = 0;
        for (uit = mSumValues.begin(); uit != mSumValues.end(); ++uit, j++)
        {
            if (isPrinted[j])
                continue;

            stream.printUnitedStat(uit->first, maxCaptionLen, uit->second, printCount);
            printCount++;
        }
    }

    class SimplePrinter {
    public:
        void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
        {
            if (stat.type == SingleStat::TIME)
            {
                printf("%-20s : %7" PRIu64 " us : %7" PRIu64 " ms  \n",
                    name.c_str(),
                    stat.mean(),
                    _roundDivUp(stat.mean(), 1000));
                return;
            }
            printf("%-20s : %7" PRIu64 "\n",
                name.c_str(),
                stat.mean());
         }
    };

    virtual void printSimple()
    {
        printf("=============================================================\n");
        SimplePrinter printer;
        printStats(printer);
        printf("=============================================================\n");
        fflush(stdout);
        SYNC_PRINT(("\n"));
    }

    class AdvancedPrinter {
    public:
        void printUnitedStat(const string &name, int length, const UnitedStat &stat, int /*lineNum*/)
        {
            if (stat.type == SingleStat::TIME)
            {
                printf("%-*s : %8" PRIu64 " us : %7" PRIu64 " ms : %7" PRIu64 " us  \n",
                    length,
                    name.c_str(),
                    stat.mean(),
                    _roundDivUp(stat.mean(), 1000),
                    stat.minimum());
                return;
            }

            printf("%-*s : %8" PRIu64 "\n",
                length,
                name.c_str(),
                stat.mean());
         }
    };

    virtual void printAdvanced()
    {
        printf("=============================================================\n");
        AdvancedPrinter printer;
        printStats(printer);
        printf("=============================================================\n");
    }


    class AdvancedSteamPrinter {
    public:
        ostream &outStream;

        AdvancedSteamPrinter(ostream &stream) : outStream(stream)
        {}

        void printUnitedStat(const string &name, int length, const UnitedStat &stat, int /*lineNum*/)
        {
            /*Well I'm just lazy*/
            char output[1000] = "";
            if (stat.type == SingleStat::TIME) {
                snprintf2buf(output,
                    "%-*s : %7" PRIu64 " us : %7" PRIu64 " ms : %7" PRIu64 " us  \n",
                    length,
                    name.c_str(),
                    stat.mean(),
                    _roundDivUp(stat.mean(), 1000),
                    stat.minimum());
            } else {
                snprintf2buf(output,
                    "%-*s : %7" PRIu64 " %7" PRIu64 "\n",
                    length,
                    name.c_str(),
                    stat.last,
                    stat.mean());
            }
            outStream << output << std::flush;
        }
    };

    virtual void printAdvanced(ostream &stream)
    {
        stream << "=============================================================\n";
        AdvancedSteamPrinter printer(stream);
        printStats(printer);
        stream << "=============================================================\n";
    }


    class WikiPrinter {
    public:
        void printUnitedStat(const string &/*name*/, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
        {
            printf("%7" PRIu64 "|", stat.mean());
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
