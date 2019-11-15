#!/bin/bash

mkdir -p Generated

source ./helper-regen.sh

output="../../wrappers/apriltag_wrapper/generated"
xml_file="../../wrappers/apriltag_wrapper/xml/apriltag.xml"

GENERATOR_BIN="../../build/tools/generator/generator"

echo -n "Building generator... "
echo "actually not"

echo -n "Running generator on ${xml_file}..."
${GENERATOR_BIN} ${xml_file}
echo "done"

echo "Making a copy of recorder classes"
copy_if_different Generated/apriltagParameters.cpp $output
copy_if_different Generated/apriltagParameters.h   $output


echo "copied"
