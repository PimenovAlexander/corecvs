target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/segmentator.h
        ${CMAKE_CURRENT_LIST_DIR}/tileGrid.h

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/segmentator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tileGrid.cpp
)

