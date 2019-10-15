target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/energyBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/imagerControl.h
        ${CMAKE_CURRENT_LIST_DIR}/cameraParameters.h
        ${CMAKE_CURRENT_LIST_DIR}/sphericalCorrectionLUT.h
    #    ${CMAKE_CURRENT_LIST_DIR}/cameraModel.h


    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/energyBuffer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imagerControl.cpp
        ${CMAKE_CURRENT_LIST_DIR}/cameraParameters.cpp
        ${CMAKE_CURRENT_LIST_DIR}/sphericalCorrectionLUT.cpp
    #    ${CMAKE_CURRENT_LIST_DIR}/cameraModel.cpp
)
