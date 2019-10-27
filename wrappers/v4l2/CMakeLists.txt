#project (V4L2Wrapper)

message(STATUS "Including V4L2Wrapper")

set (HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/V4L2CaptureDecouple.h  
    ${CMAKE_CURRENT_LIST_DIR}/V4L2Capture.h 
    ${CMAKE_CURRENT_LIST_DIR}/wrappers/v4l2/V4L2.h
)

set (SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/V4L2CaptureDecouple.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/V4L2Capture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/wrappers/v4l2/V4L2.cpp
)

set (INCLUDEPATHS
    ${INCLUDEPATHS}
    ${CMAKE_CURRENT_LIST_DIR}
)

set (LIBS
    ${LIBS}
    v4l2
)

include_directories(${INCLUDEPATHS})
add_definitions( -DWITH_LIBJPEG )





