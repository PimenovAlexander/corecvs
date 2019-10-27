#project (LibJPEGWrapper)

message(STATUS "Including LibJPEG")

set (HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.h     
)

set (SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.cpp
)

set (INCLUDEPATHS
    ${INCLUDEPATHS}
    ${CMAKE_CURRENT_LIST_DIR}
)

set (LIBS
    ${LIBS}
    jpeg
)

include_directories(${INCLUDEPATHS})
add_definitions( -DWITH_LIBJPEG )





