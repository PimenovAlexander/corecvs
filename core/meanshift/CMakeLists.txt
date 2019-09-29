target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/meanShiftCalculator.h
        ${CMAKE_CURRENT_LIST_DIR}/meanShiftWindow.h
        ${CMAKE_CURRENT_LIST_DIR}/abstractMeanShiftKernel.h

     PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/meanShiftCalculator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/meanShiftWindow.cpp
 )

