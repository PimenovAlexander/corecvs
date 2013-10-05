#pragma once
/**
 * \file log.h
 * \brief This class will be used for logging
 **/
#include <stdio.h>
#include <iostream>
#include <stdarg.h>
#include <time.h>
#ifndef _MSC_VER
#include <sys/time.h>
#endif

#include <vector>
#include <memory>
#include <sstream>
#include <string>

#include "global.h"

#include "memoryBlock.h"

using corecvs::ObjectRef;


#define L_ERROR            Log().error  (__FILE__, __LINE__, __FUNCTION__)
#define L_WARNING          Log().warning(__FILE__, __LINE__, __FUNCTION__)
#define L_INFO             Log().info   (__FILE__, __LINE__, __FUNCTION__)
#define L_DEBUG            Log().debug  (__FILE__, __LINE__, __FUNCTION__)
#define L_DDEBUG           Log().ddebug (__FILE__, __LINE__, __FUNCTION__)

#define L_ERROR_P(  ...)   Log().error  (__FILE__, __LINE__, __FUNCTION__) << Log::formatted(__VA_ARGS__)
#define L_WARNING_P(...)   Log().warning(__FILE__, __LINE__, __FUNCTION__) << Log::formatted(__VA_ARGS__)
#define L_INFO_P(   ...)   Log().info   (__FILE__, __LINE__, __FUNCTION__) << Log::formatted(__VA_ARGS__)
#define L_DEBUG_P(  ...)   Log().debug  (__FILE__, __LINE__, __FUNCTION__) << Log::formatted(__VA_ARGS__)
#define L_DDEBUG_P( ...)   Log().ddebug (__FILE__, __LINE__, __FUNCTION__) << Log::formatted(__VA_ARGS__)


class LogDrain;

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

        time_t      rawtime;
	};

	/**
	 * Message is a smart pointer to  MessageInternal. MessageInternal is usually accessed only through this structure
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
            time(&message.get()->rawtime);
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
	 * Log a message
	 **/
    void message(Message &message);

    static std::string formatted(const char *format, ... );

    static std::string msgBufToString(const char* message);

    static bool shouldWrite(LogLevel messageLevel) {
        return messageLevel >= mMinLogLevel;
    }

    static LogLevel mMinLogLevel;
    static std::vector<LogDrain *> mLogDrains;
    static int mDummy;

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
    virtual void drain(Log::Message &message) = 0;

protected:
    char   timeBuffer[32];
    cchar* time2str(time_t &time);
};

class StdStreamLogDrain : public LogDrain
{
    std::ostream &mOutputStream;

public:
    StdStreamLogDrain(std::ostream &outputStream)
        : mOutputStream(outputStream)
    {}

    virtual void drain(Log::Message &message);
};
