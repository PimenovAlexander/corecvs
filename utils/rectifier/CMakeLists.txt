set(RECTIFIER_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/estimationMethodType.h
    ${CMAKE_CURRENT_LIST_DIR}/matchingMethodType.h
    ${CMAKE_CURRENT_LIST_DIR}/optimizationMethodType.h
    ${CMAKE_CURRENT_LIST_DIR}/rectifyParameters.h
    ${CMAKE_CURRENT_LIST_DIR}/rectifyParametersControlWidget.h
    ${CMAKE_CURRENT_LIST_DIR}/universalRectifier.h
    PARENT_SCOPE
    )

set(RECTIFIER_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/rectifyParameters.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rectifyParametersControlWidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/universalRectifier.cpp
    PARENT_SCOPE
    )