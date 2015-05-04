/**
 * \file utils.cpp
 * \brief Add Comment Here
 *
 * \ingroup cppcorefiles
 * \date Apr 14, 2010
 * \author alexander
 */

#if !defined( DSP_TARGET ) && !defined( WIN32 ) && !defined( WIN64 )

#include <iostream>
#include <exception>
#include <cstdlib>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#endif

#include "utils.h"


namespace corecvs {

namespace HelperUtils {



istream &getlineSafe(istream &is, std::string &str)
{
    istream& stream = getline(is, str);

    if (str.length() != 0 &&  *str.rbegin() == '\r') {
        str.erase(str.length() - 1);
    }

    return stream;
}

bool startsWith(const string &str, const string &prefix)
{
    return (str.compare(0, prefix.size(), prefix) == 0);
}

}

#if defined( DSP_TARGET ) || defined( WIN32 ) || defined( WIN64 )
// It is possible but quite hard and usually not needed to print stack trace on Win32.
// Debugging shall be done with debugger when possible, if not, minidump is better than stack trace
// http://stackoverflow.com/questions/105659/how-can-one-grab-a-stack-trace-in-c/127012#127012
void stdTerminateHandler() {}
void setStdTerminateHandler() {}

#else

void stdTerminateHandler()
{
      void *array[10];
      size_t size;

      // get void*'s for all entries on the stack
      size = backtrace(array, 10);

      // print out all the frames to stderr
      //fprintf(stderr, "Error: signal %d:\n", sig);
      backtrace_symbols_fd(array, size, 2);
      exit(1);
}

void setStdTerminateHandler()
{
    std::set_terminate(stdTerminateHandler);
}


#endif // DSP_TARGET

} //namespace corecvs


