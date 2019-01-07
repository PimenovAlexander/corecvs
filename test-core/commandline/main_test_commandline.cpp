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
#include <generated/testBlock.h>
#include <core/reflection/dynamicObject.h>
#include <core/reflection/usageVisitor.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/reflection/commandLineSetter.h"
#include "core/reflection/commandLineGetter.h"
#include "core/math/vector/vector3d.h"

using namespace corecvs;

using std::istream;
using std::ostream;
using std::cout;


TEST(CommandLine, testAdditionalFunction)
{
    const char *argv[] = {"--bool", "--int=42", "--double=3.14", "--string=test1", "--value with spaces=999"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);
    bool   boolVal = setter.getBool("bool");
    int    intVal  = setter.getInt("int");
    double dblVal  = setter.getDouble("double");
    string strVal  = setter.getString("string", "");
    int    intSpVal= setter.getInt("value with spaces");

    //cout << "Bool:"   << b << endl;
    //cout << "Int:"    << i << endl;
    //cout << "Double:" << d << endl;

    CORE_ASSERT_TRUE(boolVal  == true,    "Bool parsing problem");
    CORE_ASSERT_TRUE(intVal   == 42,      "Int parsing problem");
    CORE_ASSERT_TRUE(dblVal   == 3.14,    "Double parsing problem");
    CORE_ASSERT_TRUE(strVal   == "test1", "String parsing problem");
    CORE_ASSERT_TRUE(intSpVal == 999,     "Int with spaces parsing problem");
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

TEST(CommandLine, testVisitorKeep)
{
    const char *argv[] = {"--x=2.4", "--y=42"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);
    Vector3dd in;
    in = Vector3dd(1.0,2.0,3.0);
    cout << "Before Loading :"  << in << std::endl;

    in.accept<CommandLineSetter>(setter);
    cout << "After Loading :"  << in << std::endl;

    CommandLineGetter getter;
    in.accept<CommandLineGetter>(getter);
    cout << getter.str() << std::endl;

    CORE_ASSERT_TRUE(in.x() ==  2.4, "Error in visitor x");
    CORE_ASSERT_TRUE(in.y() ==   42, "Error in visitor y");
    CORE_ASSERT_TRUE(in.z() ==    0, "Error in visitor z");


    in = Vector3dd(1.0,2.0,3.0);
    CommandLineSetter setterKeep(argc, argv);
    setterKeep.mPreserveValues = true;
    in.accept<CommandLineSetter>(setterKeep);
    CORE_ASSERT_TRUE(in.x() ==  2.4, "Error in visitor x");
    CORE_ASSERT_TRUE(in.y() ==   42, "Error in visitor y");
    CORE_ASSERT_TRUE(in.z() ==  3.0, "Error in visitor z");
}

TEST(CommandLine, testVisitorComplexObject0)
{
     TestBlock block;
     {
         CommandLineGetter getter;
         cout << "Visitor call" << std::endl;
         getter.visit<TestBlock>(block, block.getReflection());
         cout << getter.str() << std::endl;
     }

     {
         CommandLineGetter getter;
         cout << "Visitor call1" << std::endl;
         getter.visit<TestBlock>(block, "prefix.");
         cout << getter.str() << std::endl;
     }

     {
        CommandLineGetter getter;
        cout << "Accept call" << std::endl;
        block.accept<CommandLineGetter>(getter);
     }

}

TEST(CommandLine, testVisitorComplexObject)
{    
    TestBlock block;
    UsagePrinter printer;
    printer.printUsage(block.getReflection(), "prefix.");
    cout << " ============================================" << std::endl;

    cout << block << std::endl;

    cout << " ============================================" << std::endl;

    const char *argv[] = {"--prefix.params.String field=change"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);
    DynamicObject obj(&block);
    setter.mPreserveValues = true;
    setter.visit(obj, "prefix");

    obj.copyTo(&block);
    cout << block << std::endl;
}


TEST(CommandLine, testSubstring)
{
    const char *argv[] = {"./bin/test_cloud_match", "--icplist", "test", "test"};
    int argc = CORE_COUNT_OF(argv);

    CommandLineSetter setter(argc, argv);

    cout << setter.getBool("icp") << "  " << setter.getBool("icplist") << std::endl;

    CORE_ASSERT_TRUE(setter.getBool("icp")     == false, "False positive");
    CORE_ASSERT_TRUE(setter.getBool("icplist") == true, "False negative");

}

