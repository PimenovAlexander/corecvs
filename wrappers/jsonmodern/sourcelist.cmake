set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/jsonModernReader.h  
    ${CMAKE_CURRENT_LIST_DIR}/sources/src/json.hpp
)
include_directories(${CMAKE_CURRENT_LIST_DIR}/sources/src/)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/jsonModernReader.cpp
)