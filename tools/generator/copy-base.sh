#!/bin/bash

source ./helper-regen.sh

base="../../applications/base/"
utils="../../utils/"

copy_if_different Generated/baseParametersControlWidget.cpp  $base/
copy_if_different Generated/baseParameters.cpp               $base/generatedParameters
copy_if_different Generated/rotationPresets.h                $base/generatedParameters
copy_if_different Generated/baseParameters.h                 $base/generatedParameters

copy_if_different Generated/presentationParametersControlWidget.cpp  $base/
copy_if_different Generated/presentationParameters.cpp               $base/generatedParameters
copy_if_different Generated/presentationParameters.h                 $base/generatedParameters
copy_if_different Generated/stereoStyle.h                            $base/generatedParameters
copy_if_different Generated/flowStyle.h                              $base/generatedParameters
copy_if_different Generated/outputStyle.h                            $base/generatedParameters

