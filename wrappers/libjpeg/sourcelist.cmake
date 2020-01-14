set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.h  
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libjpegFileReader.cpp
)

add_definitions(-DWITH_LIBJPEG)

set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${JPEG_INCLUDE_DIR}
    )

set(LIBS ${LIBS} ${JPEG_LIB})
