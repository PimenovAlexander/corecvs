set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/linuxJoystickInterface.h
)
include_directories(${CMAKE_CURRENT_LIST_DIR})

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/linuxJoystickInterface.cpp
)

