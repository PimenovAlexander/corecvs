/**
 * \file main_test_logger.cpp
 * \brief This is the main file for the test logger 
 *
 * \date Sep 26, 2012
 * \author alexander
 *
 * \ingroup autotest  
 */

#ifndef TRACE
//#define TRACE
#endif

#include <string>
#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/utils/utils.h"
#include "core/utils/log.h"

using namespace std;
using namespace corecvs;

TEST(Logger, testDummy)  // TODO: add log to file and then check its content!
{
 //   L_ERROR("Test with place");

  //  Log().info() << "Hello";

    Log logger;
    Log::MessageScoped(&logger, Log::LEVEL_ERROR, "A", 0, "F");

    cout << Log::formatted("Here we go %d\n", 1, 2, "three");
   // Note that std::endl is designed to work with std::basic_ostream. Thus it is not supported
   // L_INFO << "corecvs::Log sucks" << std::endl;
}

class Foo {
public:
    Foo(int v) : data(v) {}
    int data;
};

inline std::ostream & operator<<(std::ostream &os, const Foo &o) {
    os << "foo data=" << o.data;
    return os;
}

TEST(Logger, testObjectLog)
{
    std::ostringstream os;
    {
        //auto &prev = Log::mLogDrains[0];
        {
            Log::mLogDrains.resize(0);
            Log::mLogDrains.add(new StdStreamLogDrain(os));

            Foo foo1(123);
            Foo foo2(456);
            L_INFO_P("test:") << " foo1:" << foo1 << " foo2:" << foo2; // << std::endl;
        }
        Log::mLogDrains.resize(0);
        //Log::mLogDrains.push_back(prev);
        Log::staticInit();
    }

    string out(os.str());
    string checkStr("test: foo1:foo data=123 foo2:foo data=456");

    size_t pos = out.find(checkStr);
    //cout << "pos=" << pos << " len:" << checkStr.length() << " len:" << out.length() << endl;

    CORE_ASSERT_TRUE_P(pos + checkStr.length() + 1 == out.length(), ("incorrect log content"));
    CORE_ASSERT_TRUE_P(out[out.length() - 1]       == '\n'        , ("incorrect last char"));
}

TEST(Logger, testFileLogging)
{
    string pathLog = corecvs::HelperUtils::getFullPathWithoutExt("some_test_file.something") +  "_calibration.txt";
    Log::mLogDrains.add(new FileLogDrain(pathLog));
    {
        L_INFO_P("test:") << " test1" << " test2";
        SYNC_PRINT(("Leaving test logging context\n"));
    }
    SYNC_PRINT(("Leaving logger\n"));
}
