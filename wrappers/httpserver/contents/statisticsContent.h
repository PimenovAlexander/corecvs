#ifndef STATISTICS_CONTENT_H_
#define STATISTICS_CONTENT_H_

#include "core/stats/calculationStats.h"
#include "httpContent.h"



class UnitedStatisticsContent : public HttpContent
{
public:


    UnitedStatisticsContent(corecvs::BaseTimeStatisticsCollector const &stat);
    virtual std::vector<uint8_t> getContent();

private:
    corecvs::BaseTimeStatisticsCollector mStat;
};


class StatisticsContent : public HttpContent
{
public:


    StatisticsContent(Statistics const &stat);
    virtual QByteArray getContent();

private:
    corecvs::Statistics mStat;
};


class StatisticsJSONContent : public HttpContent
{
public:
    StatisticsJSONContent(corecvs::Statistics const &stat);

    virtual std::vector<uint8_t> getContent();
    virtual std::string getContentType()
    {
        return "application/json";
    }

private:
    corecvs::Statistics mStat;
};

#endif // STATISTICS_CONTENT_H_
