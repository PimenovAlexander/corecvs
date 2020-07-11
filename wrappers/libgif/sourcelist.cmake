set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libgifFileReader.h
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libgifFileReader.cpp
)

add_definitions(-DWITH_LIBGIF)
set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${GIF_INCLUDE_DIR}
    )

set(LIBS ${LIBS} ${GIF_LIB})
