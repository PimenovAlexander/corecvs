#!/bin/bash

mkdir -p Generated

source ./helper-regen.sh

opencv="../../wrappers/opencv/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/opencv.xml..."
${GENERATOR_BIN} xml/opencv.xml
${GENERATOR_BIN} xml/opencvsgm.xml
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/openCVKLTParameters.cpp                 $opencv/generated
copy_if_different Generated/openCVKLTParameters.h                   $opencv/generated


copy_if_different Generated/openCVBMParameters.h                    $opencv/generated
copy_if_different Generated/openCVBMParameters.cpp                  $opencv/generated

#copy_if_different Generated/openCVBMParametersControlWidget.h       $opencv/generated
copy_if_different Generated/openCVBMParametersControlWidget.cpp     $opencv/generated
copy_if_different Generated/openCVBMParametersControlWidget.ui      $opencv/generated

copy_if_different Generated/openCVSGMParameters.h                   $opencv/generated
copy_if_different Generated/openCVSGMParameters.cpp                 $opencv/generated

#copy_if_different Generated/openCVSGMParametersControlWidget.h      $opencv/generated
copy_if_different Generated/openCVSGMParametersControlWidget.cpp    $opencv/generated
copy_if_different Generated/openCVSGMParametersControlWidget.ui     $opencv/generated

copy_if_different Generated/openCVSquareDetectorParameters.h        $opencv/generated
copy_if_different Generated/openCVSquareDetectorParameters.cpp      $opencv/generated


echo "copied"
