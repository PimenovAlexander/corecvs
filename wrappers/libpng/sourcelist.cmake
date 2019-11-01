set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.h  
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libpngFileReader.cpp
)

add_definitions(-DWITH_LIBPNG)
include_directories(${CMAKE_CURRENT_LIST_DIR})
link_libraries(${PNG_LIB})
