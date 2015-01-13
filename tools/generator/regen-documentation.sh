#!/bin/bash

INPUT=Generated
OUTPUT=Generated/wiki.redmine 
widget_list="base presentation"
widget_alias=("input" "presentation")

echo "" >$OUTPUT

num=0;
for widget in $widget_list; do
   echo "h2. ${widget_alias[$num]} Parameters" >>$OUTPUT
   echo "" >>$OUTPUT
   echo "!screen_${widget}.png!"               >>$OUTPUT
   cat $INPUT/${widget}ParametersControlWidget.redmine >>$OUTPUT
   echo "" >>$OUTPUT
   ((num=num+1))
done;