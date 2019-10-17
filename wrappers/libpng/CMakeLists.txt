#project (LibPNGWrapper)

message(STATUS "Including LibPNG")

set (HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.h     
)

set (SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.cpp
)

set (INCLUDEPATHS
    ${INCLUDEPATHS}
    ${CMAKE_CURRENT_LIST_DIR}
)

set (LIBS
    ${LIBS}
    png
)

include_directories(${INCLUDEPATHS})
add_definitions( -DWITH_LIBPNG )





