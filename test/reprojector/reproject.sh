#!/bin/bash

if [[ ! -e ./bin ]]; then
    echo "Call this from root folder of repository"
    exit 1
fi;

echo "Rebuilding..."
make -j 5

REPROJECT_BIN=./bin/reprojector

if [[ ! -e ${REPROJECT_BIN} ]]; then
    echo "Failed to build the application"
    exit 1
fi;

INDIR=$1

MAXNUM=10000

SEQ_PREFIX="000"
FLOW_FORMAT="png"

mkdir ${INDIR}/reprojected


echo " ----------------  Processing ---------------"

if true; then
    for (( c=0; c < ${MAXNUM}; c++ )) do

        numid1=`printf "%06d" $c`

        filename="${SEQ_PREFIX}_${numid1}.png"
        rgb1front="${INDIR}/rgb1-front/${filename}"
        output="${INDIR}/reprojected/${filename}"

        if [ ! -e $rgb1front ]; then
            echo "Current rgb file not found ${depth1}"
            break;
        fi;

        executable="${REPROJECT_BIN} \
        --carla=${INDIR} \
        --file=${filename} \
        --output=${output}"

        echo ${executable}
        ${executable}

    done;
fi;


