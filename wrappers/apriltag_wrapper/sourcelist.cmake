set (HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/apriltagDetector.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/apriltagParameters.h
        )

set (SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/apriltagDetector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/apriltagParameters.cpp
        )


include_directories(${CMAKE_CURRENT_LIST_DIR})