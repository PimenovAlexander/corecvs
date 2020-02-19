set(ALIGNMENT_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/radialCorrection.h
    ${CMAKE_CURRENT_LIST_DIR}/distortionCorrectTransform.h
    ${CMAKE_CURRENT_LIST_DIR}/camerasCalibration/camerasCalibrationFunc.h
    ${CMAKE_CURRENT_LIST_DIR}/radialFunc.h
    ${CMAKE_CURRENT_LIST_DIR}/curvatureFunc.h
    ${CMAKE_CURRENT_LIST_DIR}/angleFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/anglePointsFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/distPointsFunction.h
    ${CMAKE_CURRENT_LIST_DIR}/selectableGeometryFeatures.h
    ${CMAKE_CURRENT_LIST_DIR}/lmDistortionSolver.h
    ${CMAKE_CURRENT_LIST_DIR}/lensDistortionModelParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/pointObservation.h
    )

set(ALIGNMENT_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/radialCorrection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/camerasCalibration/camerasCalibrationFunc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/radialFunc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/curvatureFunc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/angleFunction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/anglePointsFunction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/distPointsFunction.cpp
    ${CMAKE_CURRENT_LIST_DIR}/selectableGeometryFeatures.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lmDistortionSolver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/lensDistortionModelParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pointObservation.cpp
    )

set(ALIGNMENT_HEADER_FILES ${ALIGNMENT_HEADER_FILES} PARENT_SCOPE)
set(ALIGNMENT_SOURCE_FILES ${ALIGNMENT_SOURCE_FILES} PARENT_SCOPE)