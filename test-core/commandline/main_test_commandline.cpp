/**
 * \file main_test_commandline.cpp
 * \brief This is the main file for the test commandline 
 *
 * \date aug. 21, 2015
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "global.h"

#include "commandLineSetter.h"

using namespace corecvs;

TEST(CommandLine, main)
{
    const char *argv[] = {"--bool", "--int=42", "--double=3.14", "--string=test1"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);
    bool   boolVal = setter.getBool("bool");
    int    intVal  = setter.getInt("int");
    double dblVal  = setter.getDouble("double");
    string strVal  = setter.getString("string", "");

    //cout << "Bool:"   << b << endl;
    //cout << "Int:"    << i << endl;
    //cout << "Double:" << d << endl;

    CORE_ASSERT_TRUE(boolVal == true,    "Bool parsing problem");
    CORE_ASSERT_TRUE(intVal  == 42,      "Int parsing problem");
    CORE_ASSERT_TRUE(dblVal  == 3.14,    "Double parsing problem");
    CORE_ASSERT_TRUE(strVal  == "test1", "String parsing problem");
}
