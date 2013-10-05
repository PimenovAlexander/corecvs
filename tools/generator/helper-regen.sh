#!/bin/sh

GENERATOR_BIN="./generator"

greenText="\x1b[32m"
redText="\x1b[31m"
yellowText="\x1b[33m"
blueText="\x1b[34m"
normalText="\x1b[0m"

copy_if_different () {
#    echo -n "Copy $1 to folder $2..."
    name=`basename $1`
    
    if [[ ! -f "$1" ]]; then
        echo -e "${redText} source \"$1\" doesn't exist ${normalText}"
        exit;
    fi;
    
    
    if diff -q "$1" "$2/$name" >/dev/null; then
        echo -e "=>${greenText} $name same...   no copy${normalText}"; 
    else
        echo -e -n "=>${yellowText} Copy $name...${normalText}";
        echo "$1" "$2/$name"
        cp $1 $2/$name 
        if [[ "$?" != "0" ]]; then
        	echo -e "${redText} failed ${normalText}"
        else 
        	echo -e "${yellowText} done ${normalText}"
        fi;
    fi;   
}   

