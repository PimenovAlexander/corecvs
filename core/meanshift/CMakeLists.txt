set(MEANSHIFT_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/meanShiftCalculator.h
    ${CMAKE_CURRENT_LIST_DIR}/meanShiftWindow.h
    ${CMAKE_CURRENT_LIST_DIR}/abstractMeanShiftKernel.h
    PARENT_SCOPE
    )

set(MEANSHIFT_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/meanShiftCalculator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/meanShiftWindow.cpp
    PARENT_SCOPE
    )
