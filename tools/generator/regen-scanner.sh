#!/bin/bash

mkdir -p Generated
source ./helper-regen.sh

scanner="../../applications/scanner/"

echo -n "Building generator... "
qmake && make
echo "done"

xmlfile="${scanner}/xml/scanner.xml"
echo -n "Running generator on ${xmlfile}..."
${GENERATOR_BIN} ${xmlfile}
echo "done"


echo "Making a copy of base classes"
./copy-base.sh


echo "Making a copy of recorder classes"
copy_if_different Generated/scannerParametersControlWidget.cpp     $scanner/
copy_if_different Generated/scannerParameters.cpp                  $scanner/generatedParameters
copy_if_different Generated/scannerParameters.h                    $scanner/generatedParameters

copy_if_different Generated/parametersMapperScanner.cpp  $scanner/parametersMapper
copy_if_different Generated/parametersMapperScanner.h    $scanner/parametersMapper

copy_if_different Generated/scannerParametersControlWidget.ui     $scanner/ui

echo "copied"
