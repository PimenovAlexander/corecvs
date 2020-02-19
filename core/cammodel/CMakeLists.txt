set(CAMMODEL_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/energyBuffer.h
    ${CMAKE_CURRENT_LIST_DIR}/imagerControl.h
    ${CMAKE_CURRENT_LIST_DIR}/cameraParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/sphericalCorrectionLUT.h
#    ${CMAKE_CURRENT_LIST_DIR}/cameraModel.h
    PARENT_SCOPE
    )

set(CAMMODEL_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/energyBuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imagerControl.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cameraParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sphericalCorrectionLUT.cpp
#    ${CMAKE_CURRENT_LIST_DIR}/cameraModel.cpp
    PARENT_SCOPE
    )
