target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/FCostFunction.h
        ${CMAKE_CURRENT_LIST_DIR}/flowVectorInformation.h
        ${CMAKE_CURRENT_LIST_DIR}/simulation/testSceneSimulator.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/FCostFunction.cpp
        ${CMAKE_CURRENT_LIST_DIR}/flowVectorInformation.cpp
        ${CMAKE_CURRENT_LIST_DIR}/simulation/testSceneSimulator.cpp
)

