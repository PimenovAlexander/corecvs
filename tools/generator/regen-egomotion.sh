#!/bin/bash

source ./helper-regen.sh

mkdir -p Generated

application="../../applications/egomotion/"
xml="xml/egomotion.xml"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on ${xml}..."
${GENERATOR_BIN} ${xml}
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/egomotionParameters.cpp                  $application/generated
copy_if_different Generated/egomotionParameters.h                    $application/generated
copy_if_different Generated/egomotionParametersControlWidget.ui      $application/generated/ui
copy_if_different Generated/egomotionParametersControlWidget.cpp     $application/generated/ui


copy_if_different Generated/parametersMapperEgomotion.cpp  $application/parametersMapper
copy_if_different Generated/parametersMapperEgomotion.h    $application/parametersMapper



echo "copied"
