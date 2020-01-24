HEADERS += \
        cameracalibration/flatPatternCalibrator.h    \
        cameracalibration/calibrationLocation.h      \       
        cameracalibration/cameraConstraints.h        \
        cameracalibration/calibrationDrawHelpers.h   \       
        cameracalibration/cameraModel.h              \
        cameracalibration/projection/projectionModels.h         \
        cameracalibration/projection/pinholeCameraIntrinsics.h  \
        cameracalibration/projection/equidistantProjection.h    \
        cameracalibration/projection/equisolidAngleProjection.h \
    $$PWD/projection/stereographicProjection.h \
    $$PWD/projection/projectionFactory.h \
    $$PWD/ilFormat.h \
    $$PWD/projection/omnidirectionalProjection.h


SOURCES += \
        cameracalibration/calibrationLocation.cpp      \
        cameracalibration/flatPatternCalibrator.cpp    \
        cameracalibration/cameraConstraints.cpp        \
        cameracalibration/calibrationDrawHelpers.cpp   \
        cameracalibration/cameraModel.cpp              \
        cameracalibration/projection/projectionModels.cpp         \
        cameracalibration/projection/pinholeCameraIntrinsics.cpp  \
        cameracalibration/projection/equidistantProjection.cpp    \
        cameracalibration/projection/equisolidAngleProjection.cpp \
    $$PWD/projection/stereographicProjection.cpp \
    $$PWD/projection/projectionFactory.cpp \
    $$PWD/ilFormat.cpp \
    $$PWD/projection/omnidirectionalProjection.cpp

OTHER_FILES +=$$PWD/CMakeLists.txt
