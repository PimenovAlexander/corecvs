//#include <iomanip>
//#include <unistd.h>
#include <stdexcept>
#include <iostream>

#include "log.h"

const char *Log::level_names[] =
{
   "Det. Debug",  // LEVEL_DETAILED_DEBUG, /**< Detailed log */
   "Debug",       // LEVEL_DEBUG,          /**< Debugging information is outputed */
   "Info",        // LEVEL_INFO,           /**< Normal information messages are outputed */
   "Warning",     // LEVEL_WARNING,        /**< Only warnings are reported */
   "Error"        // LEVEL_ERROR,          /**< Only errors are reported */
                  // LEVEL_LAST            /**< Last enum value for iterating*/
};

STATIC_ASSERT(CORE_COUNT_OF(Log::level_names) == Log::LEVEL_LAST, wrong_number_of_text_constants);

/**
 * It is impossible to tell when this function will be executed, so you should not log from
 * static initializers
 **/

Log::LogLevel           Log::mMinLogLevel = LEVEL_FIRST;
std::vector<LogDrain *> Log::mLogDrains;

int Log::mDummy = Log::staticInit();


int Log::staticInit()
{
    LogDrain *defaultDrain = new StdStreamLogDrain(std::cout);
    mLogDrains.push_back(defaultDrain);
//	logStream.reset(new std::ofstream(fileName.c_str(), std::ios::out | std::ios::app));
    return 0;
}

void Log::message(Message &message)
{
    for(unsigned int i = 0; i < mLogDrains.size(); i++)
    {
        mLogDrains[i]->drain(message);
    }
}

Log::Log(const LogLevel /*maxLocalLevel*/)
{
}

Log::~Log()
{
}

std::string Log::msgBufToString(const char* msg)
{
    std::string message(msg);

    if (message.size() != 0 && message[message.size() - 1] == '\n') {
        message.resize(message.size() - 1);
    }

    return message;
}

//static
std::string Log::formatted(const char *format, ...)
{
    std::string result;

    va_list marker;
    va_start(marker, format);
        size_t len = vsnprintf(NULL, 0, format, marker) + 1;    // to add a nul symbol
    va_end(marker);

    va_list marker2;
    va_start(marker2, format);
#if 1
        char* buf = new char[len];
        vsnprintf(buf, len, format, marker2);
        result.assign(buf);
        delete[] buf;
#else
        // TODO: this code inserts one dummy symbol at begin of the result?!!
        result.resize(len);
      //vsnprintf(result._Myptr(), len, format, marker2);
        vsnprintf(&result[0], len, format, marker2);
#endif
    va_end(marker2);

    return result;
}

cchar* LogDrain::time2str(time_t &time)
{
    struct tm *timeinfo = localtime(&time);

    snprintf2buf(timeBuffer, "%d, %d %d %02d:%02d:%02d",
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon  + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec
        );
    return timeBuffer;
}

void StdStreamLogDrain::drain(Log::Message &message)
{
    std::ostringstream prefix;

    prefix << time2str(message.get()->rawtime)   << ":"
           << Log::level_names[message.get()->mLevel] << " "
           << message.get()->mOriginFileName     << ":"
           << message.get()->mOriginLineNumber   << " "
           << message.get()->mOriginFunctionName << "() ";

    size_t len = prefix.str().size();
    mOutputStream << prefix.str();

    const std::string &messageString = message.get()->s.str();
    size_t pos = 0;
    do {
        if (pos != 0) {
            mOutputStream << std::string(len, ' ');
        }
        size_t posBr = messageString.find('\n', pos);

        mOutputStream << messageString.substr(pos, posBr - pos) << std::endl;

        if (posBr == std::string::npos)
            break;
        pos = posBr + 1;
    } while (true);
}


FileLogDrain::FileLogDrain(const std::string &path, bool bAppend)
    : mFile(path.c_str(), bAppend ? std::ios_base::app : std::ios_base::trunc)
{}

FileLogDrain::~FileLogDrain()
{
    mFile.flush();
    mFile.close();
}

void FileLogDrain::drain(Log::Message &message)
{
    if (mFile.is_open())
    {
        mFile << message.get()->s.str() << std::endl;
        mFile.flush();
    }
}


void LiteStdStreamLogDrain::drain(Log::Message &message) {
    mOutputStream << message.get()->s.str() << std::endl;
    mOutputStream.flush();
}
