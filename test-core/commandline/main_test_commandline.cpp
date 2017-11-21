/**
 * \file main_test_commandline.cpp
 * \brief This is the main file for the test commandline 
 *
 * \date aug. 21, 2015
 * \author alexander
 *
 * \ingroup autotest  
 **/

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/reflection/commandLineSetter.h"
#include "core/reflection/commandLineGetter.h"
#include "core/math/vector/vector3d.h"

using namespace corecvs;

TEST(CommandLine, testAdditionalFunction)
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


TEST(CommandLine, testVisitor)
{
    const char *argv[] = {"--x=2.4", "--y=42", "--z=3.14"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);
    Vector3dd in (1.0,2.0,3.0);
    cout << "Before Loading :"  << in << std::endl;

    in.accept<CommandLineSetter>(setter);
    cout << "After Loading :"  << in << std::endl;

    CommandLineGetter getter;
    in.accept<CommandLineGetter>(getter);
    cout << getter.str() << std::endl;


    CORE_ASSERT_TRUE(in.x() ==  2.4, "Error in visitor x");
    CORE_ASSERT_TRUE(in.y() ==   42, "Error in visitor y");
    CORE_ASSERT_TRUE(in.z() == 3.14, "Error in visitor z");
}

