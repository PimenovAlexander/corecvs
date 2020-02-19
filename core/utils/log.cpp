#include <iostream>

#ifdef WIN32
# include <windows.h>       // GetCurrentThreadId
#else
# include <unistd.h>
# include <sys/syscall.h>
#endif

#include "utils/log.h"
#include "reflection/commandLineSetter.h"
#include "tbbwrapper/tbbWrapper.h"
#include "filesystem/tempFolder.h"
#include "utils/utils.h"

const char *Log::level_names[] =
{
     "D.Debug"     // LEVEL_DETAILED_DEBUG, /**< Detailed log */
   , "Debug"       // LEVEL_DEBUG,          /**< Debugging information is outputed */
   , "Info"        // LEVEL_INFO,           /**< Normal information messages are outputed */
   , "Warning"     // LEVEL_WARNING,        /**< Only warnings are reported */
   , "Error"       // LEVEL_ERROR,          /**< Only errors are reported */
                   // LEVEL_LAST            /**< Last enum value for iterating*/
};

STATIC_ASSERT(CORE_COUNT_OF(Log::level_names) == Log::LEVEL_LAST, wrong_number_of_log_levels)

/**
 * It is impossible to tell when this function will be executed, so you should not log from
 * static initializers
 **/
Log::LogLevel   Log::mMinLogLevel = LEVEL_FIRST;
LogDrainsKeeper Log::mLogDrains;

int Log::mDummy = Log::staticInit();

int Log::staticInit()
{
    mLogDrains.add(new StdStreamLogDrain(std::cout));
    return 0;
}

void Log::message(Message &message)
{
    for (LogDrain *el: mLogDrains)
    {
        el->drain(message);
    }
}

int Log::getCurrentThreadId()
{
#ifdef WIN32
            return (int)GetCurrentThreadId();
#else
            return syscall(SYS_gettid);
#endif
}

Log::Log(const LogLevel /*maxLocalLevel*/)
{}

Log::~Log()
{}

//static
const char* Log::levelName(LogLevel logLevel)
{
    return (unsigned)logLevel < LEVEL_LAST ? level_names[logLevel] : "unknown";
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

void Log::addAppLog(int argc, char* argv[], cchar* logFileName, cchar *projectEnvVar)
{
    /** Detect min LogLevel and log filename from params
     */
    corecvs::CommandLineSetter setter(argc, argv);
    Log::LogLevel minLogLevel = (Log::LogLevel)setter.getInt("logLevel", Log::LEVEL_INFO);
    std::string   logFile = setter.getString("logFile", logFileName ? logFileName : "");
    auto logLevel = corecvs::HelperUtils::getEnvVar("CVSDK_LOG_LEVEL");
    if (!logLevel.empty())
    {
        minLogLevel = (Log::LogLevel)std::stoi(logLevel.c_str());
        std::cout << "Detected minLogLevel env.var.: set to " << minLogLevel << std::endl;
    }

    /** add needed log drains
    */
    cchar* progPath = argv[0];
    std::string pathApp(progPath);
    size_t pos = pathApp.rfind(PATH_SEPARATOR);
    pathApp.resize(pos + 1);                            // extract path to the program

    if (!logFile.empty())
    {
      //Log::mLogDrains.clear();
      //Log::mLogDrains.add(new LiteStdStreamLogDrain(std::cout));
        Log::mLogDrains.add(new FileLogDrain(pathApp + logFile));
    }

#ifdef GIT_VERSION
# define GCC_XSTR(value) GCC_STR(value)
# define GCC_STR(value)  #value
    cchar* git_version = GCC_XSTR(GIT_VERSION);
#else
    cchar* git_version = "unknown";
#endif

    L_INFO_P("%s app built %s %s", &progPath[pos + 1], __DATE__, __TIME__);
    L_INFO_P("%s app version %s", &progPath[pos + 1], git_version);
    Log::mMinLogLevel = LEVEL_DETAILED_DEBUG;
    L_INFO_P("App Log Level: %s", Log::levelName(minLogLevel));
    Log::mMinLogLevel = minLogLevel;

    L_INFO_P("%s", corecvs::tbbInfo().c_str());

    if (projectEnvVar != nullptr)
    {
        auto path = corecvs::TempFolder::TempFolderPath(projectEnvVar, true);  // clear it
        L_INFO_P("Working temp dir is <%s>", path.c_str());
    }


    // Some MSVC stuff code to activate the memory leak detector dump if we have >1 exits!
#ifdef USE_MSVC_DEBUG_MEM
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

/////////////////////////////////////////////////////////////////////////////

cchar* LogDrain::time2str(time_t &time)
{
    struct tm *timeinfo = localtime(&time);

    snprintf2buf(mTimeBuffer, "%d-%d-%d %02d:%02d:%02d",
        timeinfo->tm_year + 1900,
        timeinfo->tm_mon  + 1,
        timeinfo->tm_mday,
        timeinfo->tm_hour,
        timeinfo->tm_min,
        timeinfo->tm_sec
        );
    return mTimeBuffer;
}

void LogDrain::prefix2os(std::ostringstream &os, Log::Message &message)
{
    if (!mFullInfo)
    {
        //os << time2str(message.get()->mTime) << ";"
        //   << Log::levelName(message.get()->mLevel) << ";";
        return;
    }

    if (message.get()->mThreadId != 0)
        os << message.get()->mThreadId  << ";";

    os << time2str(message.get()->mTime) << ";"
       << Log::levelName(message.get()->mLevel) << ";";

    if (message.get()->mOriginFileName != NULL) {
        os << corecvs::HelperUtils::getFileNameFromFilePath(message.get()->mOriginFileName) << ";";
    }

    if (message.get()->mOriginLineNumber >= 0)
        os << message.get()->mOriginLineNumber << ";";

    if (message.get()->mOriginFunctionName != NULL)
        os << message.get()->mOriginFunctionName << "();";
}

/////////////////////////////////////////////////////////////////////////////

void StdStreamLogDrain::drain(Log::Message &message)
{
    std::ostringstream prefix;
    prefix2os(prefix, message);
    size_t len = prefix.str().size();

    mMutex.lock();
        if (len != 0) {
            mOutputStream << prefix.str() << '\t';
        }

        const std::string &messageString = message.get()->s.str();
        size_t pos = 0;
        do {
            if (pos != 0 && len != 0) {
                mOutputStream << std::string(len, ' ') << '\t';
            }
            size_t posBr = messageString.find('\n', pos);

            mOutputStream << messageString.substr(pos, posBr - pos) << std::endl;

            if (posBr == std::string::npos)
                break;
            pos = posBr + 1;

        } while (true);
        mOutputStream.flush();
    mMutex.unlock();
}

/////////////////////////////////////////////////////////////////////////////

FileLogDrain::FileLogDrain(const std::string &path, bool bAppend, bool fullInfo)
    : mPath(path)
    , mFile(path.c_str(), bAppend ? std::ios_base::app : std::ios_base::trunc)
{
    mFullInfo = fullInfo;
}

FileLogDrain::~FileLogDrain()
{
    SYNC_PRINT(("FileLogDrain::~FileLogDrain():called\n"));
    mFile.flush();
    mFile.close();

    SYNC_PRINT(("log <%s> saved\n", mPath.c_str()));
}

void FileLogDrain::drain(Log::Message &message)
{
    if (mFile.is_open())
    {
        std::ostringstream prefix;
        prefix2os(prefix, message);

        mMutex.lock();
            if (prefix.str().size() != 0) {
                mFile << prefix.str() << '\t';
            }
            mFile << message.get()->s.str() << std::endl;
            mFile.flush();
        mMutex.unlock();
    }
}
