set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.h  
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.cpp
)

add_definitions(-DWITH_LIBJPEG)
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(LIBS ${LIBS} ${JPEG_LIB})
