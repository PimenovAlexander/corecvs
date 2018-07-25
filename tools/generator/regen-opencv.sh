#!/bin/bash

source ./helper-regen.sh

opencv="../../wrappers/opencv/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/opencv.xml..."
${GENERATOR_BIN} xml/opencv.xml
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/KLTOpenCVParameters.cpp                  $opencv/generated
copy_if_different Generated/KLTOpenCVParameters.h                    $opencv/generated


echo "copied"
