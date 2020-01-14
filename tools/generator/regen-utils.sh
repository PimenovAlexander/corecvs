#!/bin/bash

source ./helper-regen.sh

GENERATOR_BIN="../../build/tools/generator/generator"

echo -n "Checking generator... "
if [ ! -f ${GENERATOR_BIN} ]; then
  echo "Generator not found"
  exit 1;
fi;
echo "done"

echo -n "Running generator on xml/graphPlot.xml..."
${GENERATOR_BIN} xml/graphPlot.xml
echo "done"

echo -n "Running generator on xml/draw3d.xml..."
${GENERATOR_BIN} xml/draw3d.xml
echo "done"

echo -n "Running generator on xml/utils.xml..."
${GENERATOR_BIN} xml/utils.xml
echo "done"

echo "Makeing a copy of base classes"
./copy-base.sh

dest="../../utils/widgets/generated"
copy_if_different Generated/graphPlotParametersControlWidget.cpp  $dest/
copy_if_different Generated/graphPlotParameters.cpp               $dest/
copy_if_different Generated/graphPlotParameters.h                 $dest/
copy_if_different Generated/graphStyle.h                          $dest/
echo "graphPlot copied"

copy_if_different Generated/patternFromPoseParameters.h         $dest/
copy_if_different Generated/patternFromPoseParameters.cpp       $dest/
echo "Generic utils copied"

dest="../../utils/3d/generated"
copy_if_different Generated/draw3dParametersControlWidget.ui   $dest/
copy_if_different Generated/draw3dParametersControlWidget.cpp  $dest/
copy_if_different Generated/draw3dParameters.cpp               $dest/
copy_if_different Generated/draw3dParameters.h                 $dest/
copy_if_different Generated/draw3dStyle.h                      $dest/
copy_if_different Generated/draw3dTextureGen.h                 $dest/

copy_if_different Generated/draw3dCameraParametersControlWidget.ui   $dest/
copy_if_different Generated/draw3dCameraParametersControlWidget.cpp  $dest/
copy_if_different Generated/draw3dCameraParameters.cpp               $dest/
copy_if_different Generated/draw3dCameraParameters.h                 $dest/


copy_if_different Generated/viMouse3dStereoStyle.h             $dest/
copy_if_different Generated/viMouse3dFlowStyle.h               $dest/

copy_if_different Generated/draw3dViMouseParametersControlWidget.ui   $dest/
copy_if_different Generated/draw3dViMouseParametersControlWidget.cpp  $dest/
copy_if_different Generated/draw3dViMouseParameters.cpp               $dest/
copy_if_different Generated/draw3dViMouseParameters.h                 $dest/

echo "draw3d copied"


# Unified 



#echo -n "Running generator on xml/opencvsgm.xml..."
#${GENERATOR_BIN} xml/opencvsgm.xml
#echo "done"
#
#dest="../../utils/corestructs/libWidgets"
#
#copy_if_different Generated/openCVSGMParametersControlWidget.ui   $dest/
#copy_if_different Generated/openCVSGMParametersControlWidget.cpp  $dest/
#copy_if_different Generated/openCVSGMParameters.cpp               $dest/
#copy_if_different Generated/openCVSGMParameters.h                 $dest/

#copy_if_different Generated/openCVBMParametersControlWidget.ui    $dest/
#copy_if_different Generated/openCVBMParametersControlWidget.cpp   $dest/
#copy_if_different Generated/openCVBMParameters.cpp                $dest/
#copy_if_different Generated/openCVBMParameters.h                  $dest/


