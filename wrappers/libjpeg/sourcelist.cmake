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
link_libraries(${JPEG_LIB})
