set(MEMORYUSE_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/memoryUsageCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/linuxMemoryUsageCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/macMemoryUsageCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/windowsMemoryUsageCalculator.h
    PARENT_SCOPE
    )

set(MEMORYUSE_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/memoryUsageCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/linuxMemoryUsageCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/macMemoryUsageCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/windowsMemoryUsageCalculator.cpp
    PARENT_SCOPE
    )