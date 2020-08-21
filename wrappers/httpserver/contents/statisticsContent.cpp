#include <string>
#include "statisticsContent.h"

using std::string;
using namespace corecvs;

StatisticsContent::StatisticsContent(const Statistics &stat) :
    mStat(stat)
{
}

StatisticsJSONContent::StatisticsJSONContent(const Statistics &stat) :
    mStat(stat)
{
}

class HTMLPrinter {
public:
    std::ostringstream &mData;

    HTMLPrinter(QByteArray &data) : mData(data) {}

    void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
    {
        double mean = 0.0;
        if (stat.number != 0)
        {
            mean = stat.sum / stat.number;
        }

        mData << "<tr>\n";
        if (stat.type == SingleStat::TIME)
        {
            QString stringTime;
            //uint64_t time = (stat.sum / stat.number + 500) / 1000;

            mData.append(
                        QString("<td>%1</td><td>%2 us</td><td>%3 ms</td><td>%4 us</td>\n")
                        .arg(name.c_str())
                        .arg(stat.last, 7)
                        .arg((stat.last + 500) / 1000, 7)
                        .arg(mean, 7));
        }
        else
        {
            mData.append(
                        QString("<td>%1</td><td></td><td>%2</td><td>%3</td>\n")
                        .arg(name.c_str())
                        .arg(stat.last, 10)
                        .arg(mean, 10));
        }

        mData.append("</tr>\n");
    }
};

class JSONPrinter {

public:
    QByteArray &mData;

    JSONPrinter(QByteArray &data) : mData(data) {}

    void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int lineNum)
    {
        double mean = 0.0;
        if (stat.number != 0)
        {
            mean = stat.sum / stat.number;
        }

        mData.append(lineNum == 0 ? "  " : " ,");
        mData.append("{\n");
        mData.append(
                    QString("    \"name\":\"%1\",\n"
                            "    \"value\":\"%2\",\n"
                            "    \"mean\":\"%3\"\n")
                    .arg(name.c_str())
                    .arg(stat.last, 7)
                    .arg(mean, 7));
        mData.append("  }\n");
    }
};

class HTMLPrinterEx {
public:
    QByteArray &mData;

    HTMLPrinterEx(QByteArray &data) : mData(data) {}

    void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int /*lineNum*/)
    {
        double mean = 0.0;
        if (stat.number != 0)
        {
            mean = stat.sum / stat.number;
        }


        mData.append("<tr>\n");
        if (stat.type == SingleStat::TIME)
        {
            QString stringTime;
            //uint64_t time = (stat.sum / stat.number + 500) / 1000;

            mData.append(
                        QString("<td>%1</td><td>%2</td><td>%3 us</td><td>%4 ms</td><td>%5 us</td>\n")
                        .arg(name.c_str())
                        .arg(stat.number)
                        .arg(stat.last, 7)
                        .arg((stat.last + 500) / 1000, 7)
                        .arg(mean, 7));
        }
        else
        {
            mData.append(
                        QString("<td>%1</td><td>%2</td><td></td><td>%3</td><td>%4</td>\n")
                        .arg(name.c_str())
                        .arg(stat.number)
                        .arg(stat.last, 10)
                        .arg(mean, 10));
        }

        mData.append("</tr>\n");
    }
};

QByteArray StatisticsContent::getContent()
{
    QByteArray data;
    //QtStatisticsCollector
    BaseTimeStatisticsCollector statCollector;
    statCollector.addStatistics(mStat);
    data.append("<div class=\"StatisticsTable\">\n");
    data.append("<table>\n");
    data.append("<tr><th>Value Name</th><th colspan=\"2\">Current Value</th><th>Mean Value</th></tr>\n");
    HTMLPrinter printer(data);
    statCollector.printStats(printer);
    data.append("</table>\n");
    data.append("</div>\n");
    return data;
}

std::vector<uint8_t> StatisticsJSONContent::getContent()
{
    QByteArray data;
    // QtStatisticsCollector
    BaseTimeStatisticsCollector statCollector;
    statCollector.addStatistics(mStat);
    data.append("{\n");
    data.append("\"data\" : [\n");
    JSONPrinter printer(data);
    statCollector.printStats(printer);
    data.append("     ]\n");
    data.append("}\n");
    return data;
}


UnitedStatisticsContent::UnitedStatisticsContent(const BaseTimeStatisticsCollector &stat) :
    mStat(stat)
{}

std::vector<uint8_t> UnitedStatisticsContent::getContent()
{
    QByteArray data;
    data.append("<div class=\"StatisticsTable\">\n");
    data.append("<table>\n");
    data.append("<tr><th>Value Name</th><th>Samples</th><th colspan=\"2\">Current Value</th><th>Mean Value</th></tr>\n");
    HTMLPrinterEx printer(data);
    mStat.printStats(printer);
    data.append("</table>\n");
    data.append("</div>\n");
    return data;
}


