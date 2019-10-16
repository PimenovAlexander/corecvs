#!/bin/bash

source ./helper-regen.sh

mkdir -p Generated

physics="../../applications/physics/"
physics_xml=${physics}/xml/physics.xml

echo -n "Building generator... "
qmake && make
echo "done"

echo -n "Running generator on ${physics_xml}..."
${GENERATOR_BIN} ${physics_xml}
echo "done"

echo "Making a copy of physics classes"
copy_if_different Generated/sceneDrawBackendType.h $physics/xml/generated
copy_if_different Generated/mixerChannelOperationParameters.h   $physics/xml/generated
copy_if_different Generated/mixerChannelOperationParameters.cpp $physics/xml/generated

copy_if_different Generated/pIDParameters.cpp $physics/xml/generated
copy_if_different Generated/pIDParameters.h   $physics/xml/generated

copy_if_different Generated/rateParameters.h   $physics/xml/generated
copy_if_different Generated/rateParameters.cpp $physics/xml/generated

copy_if_different Generated/betaflightPIDParameters.cpp $physics/xml/generated
copy_if_different Generated/betaflightPIDParameters.h   $physics/xml/generated

copy_if_different Generated/flightControllerParameters.cpp $physics/xml/generated
copy_if_different Generated/flightControllerParameters.h   $physics/xml/generated

copy_if_different Generated/flightMode.h  $physics/xml/generated

