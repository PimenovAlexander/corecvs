set(BOOSTING_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/adaBoost.h
    ${CMAKE_CURRENT_LIST_DIR}/cascadeClassifier.h
    ${CMAKE_CURRENT_LIST_DIR}/vjPattern.h
    ${CMAKE_CURRENT_LIST_DIR}/detectedObject.h
    PARENT_SCOPE
    )

set(BOOSTING_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/adaBoost.cpp
    ${CMAKE_CURRENT_LIST_DIR}/cascadeClassifier.cpp
    ${CMAKE_CURRENT_LIST_DIR}/vjPattern.cpp
    ${CMAKE_CURRENT_LIST_DIR}/detectedObject.cpp
    PARENT_SCOPE
    )