target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/cloud.h
        ${CMAKE_CURRENT_LIST_DIR}/swarmPoint.h
        ${CMAKE_CURRENT_LIST_DIR}/cloudCluster.h
        ${CMAKE_CURRENT_LIST_DIR}/clustering3d.h

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/cloud.cpp
        ${CMAKE_CURRENT_LIST_DIR}/swarmPoint.cpp
        ${CMAKE_CURRENT_LIST_DIR}/cloudCluster.cpp
        ${CMAKE_CURRENT_LIST_DIR}/clustering3d.cpp
    )


