#!/bin/bash

source ./helper-regen.sh

copter="../../applications/coptercontrol/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/copter.xml..."
${GENERATOR_BIN} xml/copter.xml
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/copterControlWidget.ui     $copter/generatedParameters

copy_if_different Generated/copterControlWidget.cpp     $copter/
copy_if_different Generated/copter.cpp                  $copter/generatedParameters
copy_if_different Generated/copter.h                    $copter/generatedParameters

copy_if_different Generated/parametersMapperCopter.cpp  $copter/parametersMapper
copy_if_different Generated/parametersMapperCopter.h    $copter/parametersMapper

echo "copied"
