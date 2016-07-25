/**
 * \file util.c
 * \brief TODO
 *
 * \date Mar 23, 2010
 * \author alexander
 */
#if defined(DSP_TARGET) || defined(WIN32)

#include "global.h"

/**
 *  It is possible but quite hard and usually not needed to print stack trace on Win32.
 *  Debugging shall be done with debugger when possible, if not, minidump is better than stack trace
 *  http://stackoverflow.com/questions/105659/how-can-one-grab-a-stack-trace-in-c/127012#127012
 **/

void printStackTrace() {}
void setSegVHandler() {}

#else

#include <stdint.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#include "global.h"

#if 0 //defined(__GNUC__) && !defined(WIN32)
void prettyTraceHandler(int sig)
{
    fprintf(stderr, "Error: signal %d:\n", sig);

    fprintf(stderr, "Stack trace:\n");

    void* frameArray[64];

    int frameNum = backtrace(frameArray, CORE_COUNT_OF(frameArray));

    if (frameNum == 0) {
        fprintf(stderr, "    -> no frames.\n");
    }

    char** frameNames = backtrace_symbols(frameArray, frameNum);
    char funcname[255];

    int i;
    for (i = 1; i < frameNum; i++)
    {
        char *beginName = 0;
        char *begin = 0;
        char *end = 0;

        char *p;
        for (p = frameNames[i]; *p != 0; p++)
        {
            if (*p == '(')
                beginName = p;
            else if (*p == '+')
                begin = p;
            else if (*p == ')' && begin) {
                end = p;
                break;
            }
        }

        if (beginName && begin && end
            && beginName < begin)
        {
            *beginName++ = '\0';
            *begin++ = '\0';
            *end = '\0';

            int status;
            char* ret = abi::__cxa_demangle(
                    begin_name,
                    funcname,
                    &funcnamesize,
                    &status);
            if (status == 0) {
                funcname = ret;
                fprintf(stderr, "  %s : %s+%s\n",
                        frameNames[i], funcname, begin_offset);
            }
            else {
                fprintf(stderr, "  %s : %s()+%s\n",
                        frameName[i], beginName, beginOffset);
            }
        }
        else
        {
            fprintf(stderr, "  -> mangled %s\n", frameNames[i]);
        }
    }
    free(frameName);
}
#endif

void printStackTrace()
{
    void *arr[10];

    // get void*'s for all entries on the stack
    size_t size = backtrace(arr, CORE_COUNT_OF(arr));

    // print out all the frames to stderr
    fprintf(stderr, "StackTrace:\n");

    backtrace_symbols_fd(arr, size, 2);
}

static void stackTraceHandler(int sig)
{
    fprintf(stderr, "stackTraceHandler (signal=%d):\n", sig);
    printStackTrace();
    exit(2);                                // stop the execution immediately
}

void setSegVHandler()
{
    signal(SIGSEGV, stackTraceHandler);   // install our handler
}

#endif // defined(DSP_TARGET) || defined(WIN32)
