#!/bin/bash

source ./helper-regen.sh

recorder="../../applications/recorder/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/recorder.xml..."
${GENERATOR_BIN} xml/recorder.xml 
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/recorderControlWidget.cpp     $recorder/
copy_if_different Generated/recorder.cpp                  $recorder/generatedParameters
copy_if_different Generated/recorder.h                    $recorder/generatedParameters

copy_if_different Generated/parametersMapperRecorder.cpp  $recorder/parametersMapper
copy_if_different Generated/parametersMapperRecorder.h    $recorder/parametersMapper

echo "copied"