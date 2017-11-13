#pragma once
/**
 * \file log.h
 * \brief This class will be used for logging
 **/
#include "core/utils/global.h"

#include <stdio.h>
#include <iostream>
#include <stdarg.h>
#include <time.h>
#ifndef _MSC_VER
# include <sys/time.h>
#endif
#ifdef WIN32
# include <windows.h>       // GetCurrentThreadId
#else
# include <unistd.h>
# include <sys/syscall.h>
#endif

#include <fstream>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <mutex>        // std::mutex

#include "core/buffers/memory/memoryBlock.h"

using corecvs::ObjectRef;


#define L_ERROR            ::Log().error  (__FILE__, __LINE__, __FUNCTION__)
#define L_WARNING          ::Log().warning(__FILE__, __LINE__, __FUNCTION__)
#define L_INFO             ::Log().info   (__FILE__, __LINE__, __FUNCTION__)
#define L_DEBUG            ::Log().debug  (__FILE__, __LINE__, __FUNCTION__)
#define L_DDEBUG           ::Log().ddebug (__FILE__, __LINE__, __FUNCTION__)

#define L_ERROR_P(  ...)   ::Log().error  (__FILE__, __LINE__, __FUNCTION__) << ::Log::formatted(__VA_ARGS__)
#define L_WARNING_P(...)   ::Log().warning(__FILE__, __LINE__, __FUNCTION__) << ::Log::formatted(__VA_ARGS__)
#define L_INFO_P(   ...)   ::Log().info   (__FILE__, __LINE__, __FUNCTION__) << ::Log::formatted(__VA_ARGS__)
#define L_DEBUG_P(  ...)   ::Log().debug  (__FILE__, __LINE__, __FUNCTION__) << ::Log::formatted(__VA_ARGS__)
#define L_DDEBUG_P( ...)   ::Log().ddebug (__FILE__, __LINE__, __FUNCTION__) << ::Log::formatted(__VA_ARGS__)


class LogDrain;

class LogDrainDeleter
{
    bool mNeedDel;
public:
    LogDrainDeleter(bool needDel = true) : mNeedDel(needDel) {}

    template<typename T>
    void operator()(T* p)
    {
        CORE_UNUSED(p);
        if (mNeedDel)
            delete p;
    }
};

class LogDrainsKeeper : public std::vector<LogDrain *>
{
public:
    std::mutex mMutex;

    LogDrainsKeeper() {}
   ~LogDrainsKeeper()
    {
        for(auto it = begin(); it != end(); ++it)
        {
            delete_safe(*it);
        }
    }

   void add(LogDrain* p) {
       mMutex.lock();
       push_back(p);
       mMutex.unlock();
   }

   void detach(LogDrain* p) {
       mMutex.lock();
       const auto &it = std::find(begin(), end(), p);
       if (it != end()){
           erase(it);
       }
       mMutex.unlock();
   }

};

/** \class Log
 *
 *  \brief Class that controls the log
 */
class Log
{
public:
    /** Log detail level */
    enum LogLevel {
          LEVEL_FIRST = 0
        , LEVEL_DETAILED_DEBUG = LEVEL_FIRST  /**< Detailed log */
        , LEVEL_DEBUG                         /**< Debugging information is outputed */
        , LEVEL_INFO                          /**< Normal information messages are outputed */
        , LEVEL_WARNING                       /**< Only warnings are reported */
        , LEVEL_ERROR                         /**< Only errors are reported */
        , LEVEL_LAST                          /**< Last enum value for iterating */
    };

    static const char *level_names[];

    Log(const LogLevel maxLocalLevel = LEVEL_ERROR);
   ~Log();


    /**
     * This class describes message of the log. It is always accessed through
     * smart pointers and could be as large as needed. Its lifetime depends on the
     * connected drains.
     **/
    class MessageInternal
    {
        friend class Log;
    public:

        /**
         * send to stream operator
         */
        template<typename T> MessageInternal& operator<< (const T& t) {
            s << t;
            return *this;
        }

        MessageInternal()
        {
            mLog = NULL;
        }

        /* Block copying of the internal object */
        MessageInternal(const MessageInternal&);
        MessageInternal& operator= (const MessageInternal&);

        std::ostringstream s;
        Log*        mLog;
        LogLevel    mLevel;

        /**
         *  Information about the position of the log
         */
        cchar      *mOriginFileName;
        int         mOriginLineNumber;
        cchar      *mOriginFunctionName;
        int         mThreadId;
        time_t      mTime;
    };

    /**
     * Message is a smart pointer to MessageInternal. MessageInternal is usually accessed only through this structure
     **/
    typedef ObjectRef<MessageInternal> Message;

    /**
     *   If we want to use qDebug style for debugging we need an object that will be destroyed when leaving the scope.
     *   This will trigger the flush.
     **/
    class MessageScoped
    {
    public:
        Message message;

        MessageScoped(
            Log       *log
            , LogLevel level
            , cchar *originFileName     = NULL
            , int    originLineNumber   = -1
            , cchar *originFunctionName = NULL)
        {
            message.allocate();
            message.get()->mLevel              = level;
            message.get()->mLog                = log;
            message.get()->mOriginFileName     = originFileName;
            message.get()->mOriginFunctionName = originFunctionName;
            message.get()->mOriginLineNumber   = originLineNumber;
#ifdef WIN32
            message.get()->mThreadId = (int)GetCurrentThreadId();
#else
            message.get()->mThreadId = syscall(SYS_gettid);
#endif
            time(&message.get()->mTime);
        }

        ~MessageScoped()
        {
            DOTRACE(("Scope is closing, sending data to log\n"));
            if (Log::shouldWrite(message.get()->mLevel)) {
                message.get()->mLog->message(message);
            }
            DOTRACE(("Scope is closed, data sent\n"));
        }

        template<typename T> MessageScoped& operator<< (const T& t) {
            (*message.get()) << t;
            return *this;
        }
    };


    MessageScoped error(const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this, LEVEL_ERROR, fileName, lineNumber, functionName);
    }

    /**
     * Create warning
     **/
    MessageScoped warning(const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this, LEVEL_WARNING, fileName, lineNumber, functionName);
    }

    /**
     * Create info
     **/
    MessageScoped info(const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this,  LEVEL_INFO, fileName, lineNumber, functionName);
    }

    /**
     * Create debug trace
     **/
    MessageScoped debug(const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this,  LEVEL_DEBUG, fileName, lineNumber, functionName);
    }

    /**
     * Create detailed debug trace
     **/
    MessageScoped ddebug(const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this,  LEVEL_DETAILED_DEBUG, fileName, lineNumber, functionName);
    }

    /**
     * Create trace with the given level
     **/
    MessageScoped log(LogLevel level, const char *fileName = NULL, int lineNumber = -1, const char *functionName = NULL)
    {
        return MessageScoped(this, level, fileName, lineNumber, functionName);
    }

    /**
     * Log a message
     **/
    void                message(Message &message);

    static std::string  formatted(const char *format, ... );

  //static std::string  msgBufToString(const char* message);

    static const char*  levelName(LogLevel logLevel);

    static bool         shouldWrite(LogLevel messageLevel) { return messageLevel >= mMinLogLevel; }

    /** add needed log drains for the app */
    static void         addAppLog(int argc, char* argv[], cchar* logFileName = NULL);

    static LogLevel         mMinLogLevel;
    static LogDrainsKeeper  mLogDrains;
    static int              mDummy;

public:
    /** This is a static init function */
    static int staticInit();
};

template<typename T>
Log::Message operator<< (Log::Message msg, const T& t) {
    *(msg.get()) << t;
    return msg;
}

/** \class LogDrain
 *
 *  \brief Interface class for any logger device that accept messages
 */
class LogDrain
{
public:
    virtual ~LogDrain() {}

    virtual void drain(Log::Message &message) = 0;

protected:
    std::mutex     mMutex;
    bool           mFullInfo;
    char           mTimeBuffer[32];

    cchar*         time2str(time_t &time);
    void           prefix2os(std::ostringstream &os, Log::Message &message);
};

class StdStreamLogDrain : public LogDrain
{
    std::ostream &mOutputStream;

public:
    StdStreamLogDrain(std::ostream &outputStream, bool fullInfo = true)
        : mOutputStream(outputStream)
    {
        mFullInfo = fullInfo;
    }
    virtual ~StdStreamLogDrain() {}

    virtual void drain(Log::Message &message);
};


class FileLogDrain : public LogDrain
{
    std::ofstream  mFile;

public:
    FileLogDrain(const std::string& path, bool bAppend = false, bool fullInfo = true);
    virtual ~FileLogDrain();

    virtual void drain(Log::Message &message);
};
