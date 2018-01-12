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
TIME="command time --quiet -f '%E'"

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

 rm timer.txt
 printf "Will run %*s " $len $test
 $TIME -o timer.txt $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
 result=$?
 echo -n "res:$result "

 $TIME -o timer1.txt $VALGRIND --error-exitcode=7 $EXECUTABLE --gtest_filter=$test >/dev/null 2>/dev/null
 vresult=$?
 echo -n "val:$vresult "

 if [[ -f timer.txt ]]; then 
	elapsed=`cat timer.txt | grep -o "[0-9\\.:]*" `
	echo -n " ${elapsed} "
 fi

 if [[ -f timer1.txt ]]; then
        elapsed=`cat timer1.txt | grep -o "[0-9\\.:]*" `
        echo -n " ${elapsed} "
 fi

 if [[ ("$result" == "0") && ("$vresult" == "0") ]]; then
     echo -n -e "${greenText}Done${normalText}\n";
 else
     echo -n -e "${redText}Done${normalText}\n";
 fi;
done;


