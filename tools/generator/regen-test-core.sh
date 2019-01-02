#!/bin/bash

source ./helper-regen.sh

mkdir -p Generated

dest="../../test-core/generated/"

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on xml/test.xml  ..."
${GENERATOR_BIN} xml/test.xml
echo "done"

copy_if_different Generated/testEnum.h       $dest
copy_if_different Generated/testSubClass.h   $dest
copy_if_different Generated/testClass.h      $dest
copy_if_different Generated/testBlock.h      $dest

copy_if_different Generated/testSubClass.cpp $dest
copy_if_different Generated/testClass.cpp    $dest
copy_if_different Generated/testBlock.cpp    $dest

