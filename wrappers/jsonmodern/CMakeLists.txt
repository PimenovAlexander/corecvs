#project (ModernJSONWrapper)

message(STATUS "Including Json For Modern C++")

set (HEADERS
    ${HEADERS}
    ${CMAKE_CURRENT_LIST_DIR}/jsonModernReader.h 
    ${CMAKE_CURRENT_LIST_DIR}/sources/src/json.hpp
)

set (SOURCES
    ${SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/jsonModernReader.cpp
)

set (INCLUDEPATHS
    ${INCLUDEPATHS}
    ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/sources/src
)

include_directories(${INCLUDEPATHS})
add_definitions( -DWITH_JSONMODERN )


