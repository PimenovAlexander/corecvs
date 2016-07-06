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

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "log.h"


using namespace std;

TEST(Logger, testDummy)  // TODO: add log to file and then check its content!
{
 //   L_ERROR("Test with place");

  //  Log().info() << "Hello";

    Log logger;
    Log::MessageScoped(&logger, Log::LEVEL_ERROR, "A", 0, "F");

    cout << Log::formatted("Here we go %d\n", 1, 2, "three");
}

class Foo {
public:
    Foo(int v) : data(v) {}
    int data;
};

inline ostream & operator<<(ostream &os, const Foo &o) {
    os << "foo data=" << o.data;
    return os;
}

TEST(Logger, testObjectLog)
{
    ostringstream os;
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
