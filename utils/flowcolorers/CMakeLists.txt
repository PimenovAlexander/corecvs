set(FLOWCOLORERS_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/abstractFlowColorer.h
    ${CMAKE_CURRENT_LIST_DIR}/flowColorer.h
    ${CMAKE_CURRENT_LIST_DIR}/stereoColorer.h
    PARENT_SCOPE
    )

set(FLOWCOLORERS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/abstractFlowColorer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/flowColorer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stereoColorer.cpp
    PARENT_SCOPE
    )