#!/bin/bash

GENERATOR_DIR="../../tools/generator"
if [[ ! -d  ${GENERATOR_DIR} ]]; then
    echo "No generator dir <${GENERATOR_DIR}>"
    exit 1;
fi;

source ${GENERATOR_DIR}/helper-regen.sh

echo -n "Building generator... "
pushd .
cd ${GENERATOR_DIR}
qmake && make
popd
echo "done"

GENERATOR_BIN=${GENERATOR_DIR}/${GENERATOR_BIN}

echo "Path to generator \"${GENERATOR_BIN}\""

CORE_DIR="../../core"
GEN_DIR="./Generated"
TARGET_DIR="./gen"

mkdir -p $GEN_DIR

${GENERATOR_BIN} param.xml

copy_if_different ${GEN_DIR}/error3PointParameters.cpp  ${TARGET_DIR}
copy_if_different ${GEN_DIR}/error3PointParameters.h    ${TARGET_DIR}

copy_if_different ${GEN_DIR}/targetType3Point.h      ${TARGET_DIR}

copy_if_different ${GEN_DIR}/vector2dParameters.cpp  ${TARGET_DIR}
copy_if_different ${GEN_DIR}/vector2dParameters.h    ${TARGET_DIR}

copy_if_different ${GEN_DIR}/vector3dParameters.cpp  ${TARGET_DIR}
copy_if_different ${GEN_DIR}/vector3dParameters.h    ${TARGET_DIR}


copy_if_different ${GEN_DIR}/errorEstimatorBlockBase.cpp  ${TARGET_DIR}
copy_if_different ${GEN_DIR}/errorEstimatorBlockBase.h    ${TARGET_DIR}




