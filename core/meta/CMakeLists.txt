target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/astNode.h
        ${CMAKE_CURRENT_LIST_DIR}/packedDerivative.h
        ${CMAKE_CURRENT_LIST_DIR}/floatJIT.h
        ${CMAKE_CURRENT_LIST_DIR}/astOptimize.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/astNode.cpp
        ${CMAKE_CURRENT_LIST_DIR}/packedDerivative.cpp
        ${CMAKE_CURRENT_LIST_DIR}/floatJIT.cpp
        ${CMAKE_CURRENT_LIST_DIR}/astOptimize.cpp
)

