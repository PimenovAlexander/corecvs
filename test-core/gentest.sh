#!/bin/bash
testName=$1
currentDate=`date "+%b %d, %Y"`
userName=`whoami`

greenText="\x1b[32m"
redText="\x1b[31m"
normalText="\x1b[0m"

if [ "x$testName" = "x" ]; then
echo -e "${redText}Usage $0 <testName>  ${normalText}";
exit 1;
fi;

 
echo -e -n "${greenText}Will Create test with name \"${testName}\" ${normalText}..."

mkdir -p ${testName}


#cat >${testName}/${testName}.pro  <<END_TEXT
##################################################################
# ${testName}.pro created on ${currentDate}
# This is a file for QMAKE that allows to build the test ${testName}
#
##################################################################
#include(../testsCommon.pri)
#
#TARGET = test_${testName}
#
#SOURCES += main_test_${testName}.cpp
#
#END_TEXT

cat >${testName}/main_test_${testName}.cpp <<END_TEXT
/**
 * \file main_test_${testName}.cpp
 * \brief This is the main file for the test ${testName} 
 *
 * \date ${currentDate}
 * \author ${userName}
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"


using namespace std;

TEST(${testName}, test${testName})
{
    cout << "Starting test <${testName}>" << endl;
    cout << "Test <${testName}> PASSED" << endl;
}
END_TEXT

echo >>test-core.pro -e "\t${testName}/main_test_${testName}.cpp \\"

echo -e "${redText}done${normalText}"
 
