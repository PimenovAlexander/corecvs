set(UTILS_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/global.h
    ${CMAKE_CURRENT_LIST_DIR}/stdint_win.h
    ${CMAKE_CURRENT_LIST_DIR}/preciseTimer.h
    ${CMAKE_CURRENT_LIST_DIR}/propertyList.h
    ${CMAKE_CURRENT_LIST_DIR}/visitors/propertyListVisitor.h
    ${CMAKE_CURRENT_LIST_DIR}/utils.h
    ${CMAKE_CURRENT_LIST_DIR}/visitors/basePathVisitor.h
    ${CMAKE_CURRENT_LIST_DIR}/log.h
    ${CMAKE_CURRENT_LIST_DIR}/countedPtr.h
    ${CMAKE_CURRENT_LIST_DIR}/atomicOps.h
    ${CMAKE_CURRENT_LIST_DIR}/typesafeBitmaskEnums.h
    ${CMAKE_CURRENT_LIST_DIR}/statusTracker.h
#    ${CMAKE_CURRENT_LIST_DIR}/abstractImageNamer.h
#    ${CMAKE_CURRENT_LIST_DIR}/statusTrackerCatcher.h
    ${CMAKE_CURRENT_LIST_DIR}/debuggableBlock.h
    PARENT_SCOPE
    )

set(UTILS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/memhooks.c
    ${CMAKE_CURRENT_LIST_DIR}/util.c
    ${CMAKE_CURRENT_LIST_DIR}/preciseTimer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/propertyList.cpp
    ${CMAKE_CURRENT_LIST_DIR}/visitors/propertyListVisitor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/visitors/basePathVisitor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils.cpp
    ${CMAKE_CURRENT_LIST_DIR}/log.cpp
    ${CMAKE_CURRENT_LIST_DIR}/statusTracker.cpp
#    ${CMAKE_CURRENT_LIST_DIR}/abstractImageNamer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/debuggableBlock.cpp
    PARENT_SCOPE
    )
