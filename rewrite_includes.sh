#!/bin/bash
CORETOP=.
COREDIR=core

LIST=`cd $CORETOP; find $COREDIR -name "*.h"`

declare -A replacement

for header in $LIST; do
   shortname=`basename $header`
   echo $shortname to $header
   
   replacement[$shortname]=$header
done;

INPUT_DIR=$1


for file in `find $INPUT_DIR -name "*.cpp" -or -name "*.h"`; do
   echo Patching $file...
   
   for header in $LIST; do
        shortname=`basename $header`
        
        sed -i "s@#include \"$shortname\"@#include \"$header\"@g" $file
        sed -i "s@#include \"generated/$shortname\"@#include \"$header\"@g" $file

   done;

done;
