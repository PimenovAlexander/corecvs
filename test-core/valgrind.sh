#!/bin/bash

greenText="\x1b[32m"
redText="\x1b[31m"
yellowText="\x1b[33m"
blueText="\x1b[34m"
normalText="\x1b[0m"


echo "Starting test run..."

BINDIR="./bin"
EXECUTABLE="$BINDIR/test_core"
VALGRIND="valgrind"

raw_list="`$EXECUTABLE --gtest_list_tests`"

test_list=`echo "$raw_list" | awk '/^[^ ]/ { package = $1; next; } {print package $1}'`

((len=0));
for test in $test_list; do
   if [[ $len -lt ${#test} ]]; then
      ((len = ${#test}));
   fi;
done;

echo $len;

for test in $test_list; do
 if echo $test | grep "\.DISABLED" >/dev/null; then
    printf "DISABLED %*s \n" $len $test
    continue;
 fi;

 printf "Will run %*s " $len $test
 $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
 result=$?
 echo -n "res:$result "

 $VALGRIND --error-exitcode=7 $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
 vresult=$?
 echo -n "val:$vresult "

 if [[ ("$result" == "0") && ("$vresult" == "0") ]]; then
     echo -n -e "${greenText}Done${normalText}\n";
 else
     echo -n -e "${redText}Done${normalText}\n";
 fi;
done;


