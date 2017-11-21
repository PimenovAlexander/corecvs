#!/bin/bash

source ./helper-regen.sh

merger="../../applications/merger/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/merger.xml..."
${GENERATOR_BIN} xml/merger.xml
echo "done"


#echo "Making a copy of base classes"
#./copy-base.sh


echo "Making a copy of merger classes"
copy_if_different Generated/mergerControlWidget.cpp     $merger/
copy_if_different Generated/mergerControlWidget.ui      $merger/ui
copy_if_different Generated/merger.cpp                  $merger/generatedParameters
copy_if_different Generated/merger.h                    $merger/generatedParameters

copy_if_different Generated/mergerUndistMethod.h        $merger/generatedParameters


copy_if_different Generated/parametersMapperMerger.cpp  $merger/parametersMapper
copy_if_different Generated/parametersMapperMerger.h    $merger/parametersMapper

echo "copied"
