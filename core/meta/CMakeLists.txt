set(META_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/astNode.h
    ${CMAKE_CURRENT_LIST_DIR}/packedDerivative.h
    ${CMAKE_CURRENT_LIST_DIR}/floatJIT.h
    ${CMAKE_CURRENT_LIST_DIR}/astOptimize.h
    PARENT_SCOPE
    )

set(META_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/astNode.cpp
    ${CMAKE_CURRENT_LIST_DIR}/packedDerivative.cpp
    ${CMAKE_CURRENT_LIST_DIR}/floatJIT.cpp
    ${CMAKE_CURRENT_LIST_DIR}/astOptimize.cpp
    PARENT_SCOPE
    )
