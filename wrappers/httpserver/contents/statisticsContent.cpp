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

    HTMLPrinter(std::ostringstream &data) : mData(data) {}

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
            char buffer[400];
            snprintf2buf(buffer, "<td>%s</td><td>%7d us</td><td>%7d ms</td><td>%7d us</td>\n",
                            name.c_str(),
                            stat.last,
                            (stat.last + 500) / 1000,
                            mean
                         );
            mData << buffer;

        }
        else
        {
            char buffer[400];
            snprintf2buf(buffer,"<td>%s</td><td></td><td>%10d</td><td>%10d</td>\n",
                            name.c_str(),
                            stat.last,
                            mean
                         );
             mData << buffer;

        }

        mData << "</tr>\n";
    }
};

class JSONPrinter {

public:
    std::ostringstream &mData;

    JSONPrinter(std::ostringstream &data) : mData(data) {}

    void printUnitedStat(const string &name, int /*length*/, const UnitedStat &stat, int lineNum)
    {
        double mean = 0.0;
        if (stat.number != 0)
        {
            mean = stat.sum / stat.number;
        }

        mData << (lineNum == 0 ? "  " : " ,");
        mData << "{\n";
        char buffer[400];
        snprintf2buf(buffer,
           "    \"name\":\"%s\",\n"
           "    \"value\":\"%7d\",\n"
           "    \"mean\":\"%7d\"\n",
           name.c_str(), stat.last, mean);
        mData << "  }\n";
    }
};

class HTMLPrinterEx {
public:
    std::ostringstream &mData;

    HTMLPrinterEx(std::ostringstream &data) : mData(data) {}

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
            char buffer[400];

            snprintf2buf(buffer, "<td>%s</td><td>%d</td><td>%7d us</td><td>%7d ms</td><td>%7d us</td>\n",
                        name.c_str(),
                        stat.number,
                        stat.last,
                        (stat.last + 500) / 1000,
                        mean);
            mData << buffer;
        }
        else
        {
            char buffer[400];
             snprintf2buf(buffer, "<td>%s</td><td>%d</td><td></td><td>%10d</td><td>%10d</td>\n",
                        name.c_str(),
                        stat.number,
                        stat.last,
                        mean);
            mData << buffer;
        }

        mData << "</tr>\n";
    }
};

std::vector<uint8_t> StatisticsContent::getContent()
{
    std::ostringstream data;

    //QtStatisticsCollector
    BaseTimeStatisticsCollector statCollector;
    statCollector.addStatistics(mStat);
    data << "<div class=\"StatisticsTable\">\n";
    data << "<table>\n";
    data << "<tr><th>Value Name</th><th colspan=\"2\">Current Value</th><th>Mean Value</th></tr>\n";
    HTMLPrinter printer(data);
    statCollector.printStats(printer);
    data << "</table>\n";
    data << "</div>\n";

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}

std::vector<uint8_t> StatisticsJSONContent::getContent()
{
    std::ostringstream data;

    BaseTimeStatisticsCollector statCollector;
    statCollector.addStatistics(mStat);
    data << "{\n";
    data << "\"data\" : [\n";
    JSONPrinter printer(data);
    statCollector.printStats(printer);
    data << "     ]\n";
    data << "}\n";

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}


UnitedStatisticsContent::UnitedStatisticsContent(const BaseTimeStatisticsCollector &stat) :
    mStat(stat)
{}

std::vector<uint8_t> UnitedStatisticsContent::getContent()
{
    std::ostringstream data;

    data << "<div class=\"StatisticsTable\">\n";
    data << "<table>\n";
    data << "<tr><th>Value Name</th><th>Samples</th><th colspan=\"2\">Current Value</th><th>Mean Value</th></tr>\n";
    HTMLPrinterEx printer(data);
    mStat.printStats(printer);
    data << "</table>\n";
    data << "</div>\n";

    std::string str = data.str();
    return std::vector<uint8_t>(str.begin(), str.end());
}


