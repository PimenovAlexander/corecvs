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

totalrun=0
totaldis=0
totalpassed=0
totalfailed=0

for test in $test_list; do
 if echo $test | grep "\.DISABLED" >/dev/null; then
    printf "DISABLED %*s \n" $len $test
    ((totaldis++));
    continue;
 fi;

 ((totalrun++));
 
 printf "Will run %*s " $len $test
 $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
 result=$?
 echo -n "res:$result "


 vresult=0
 if [[ $1 != "0" ]]; then 
   $VALGRIND --error-exitcode=7 $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
   vresult=$?
   echo -n "val:$vresult "
 fi

 if [[ ("$result" == "0") && ("$vresult" == "0") ]]; then
     ((totalpassed++));
     echo -n -e "${greenText}Done${normalText}\n";
 else
     ((totalfailed++));
      echo -n -e "${redText}Done${normalText}\n";
 fi;
done;

echo -e "Total Runned: " $totalrun
echo -e "Total Runned: ${greenText}${totalpassed}${normalText}\n"
echo -e "Total Runned: ${redText}${totalfailed}${normalText}\n"
echo -e "Total Disabled: " $totaldis


