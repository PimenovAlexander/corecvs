#!/bin/bash

source ./helper-regen.sh

dest="../../applications/base/"

echo -n "Building generator... "
qmake-qt4 && make
echo "done"

echo -n "Running generator on xml/baseStub.xml..."
${GENERATOR_BIN} xml/baseStub.xml  
echo "done"

echo "Making a copy of base classes"
./copy-base.sh

copy_if_different Generated/parametersMapperBase.cpp          $dest/parametersMapper
copy_if_different Generated/parametersMapperBase.h            $dest/parametersMapper

# hand-written
echo "copied"


echo -n "Running generator on xml/rectify.xml..."
${GENERATOR_BIN} xml/rectify.xml  
echo "done"

rectifier_dest="../../utils/rectifier"

copy_if_different Generated/estimationMethodType.h              $rectifier_dest
copy_if_different Generated/optimizationMethodType.h            $rectifier_dest 
copy_if_different Generated/rectifyParameters.cpp               $rectifier_dest
copy_if_different Generated/rectifyParameters.h                 $rectifier_dest
copy_if_different Generated/rectifyParametersControlWidget.cpp  $rectifier_dest

