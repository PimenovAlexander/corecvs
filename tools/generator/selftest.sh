#!/bin/bash

qmake -r generator.pro
make

mkdir -p Generated
./generator xml/test.xml


COREDIR=../../core
INCLUDES="-I${COREDIR}/reflection/ -I${COREDIR}/utils/"

echo g++ -c -std=c++0x ${INCLUDES} Generated/testClass.cpp
g++ -c -std=c++0x ${INCLUDES} Generated/testClass.cpp
g++ -c -std=c++0x ${INCLUDES} Generated/testSubClass.cpp

g++ -c -std=c++0x ${INCLUDES} Generated/testBlock.cpp

