target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/processor6D.h
        ${CMAKE_CURRENT_LIST_DIR}/dummyFlowProcessor.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/processor6D.cpp
        ${CMAKE_CURRENT_LIST_DIR}/dummyFlowProcessor.cpp
)
