set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.h  
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.cpp
)

add_definitions(-DWITH_LIBPNG)
set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${PNG_INCLUDE_DIR}
    )

set(LIBS ${LIBS} ${PNG_LIB})
