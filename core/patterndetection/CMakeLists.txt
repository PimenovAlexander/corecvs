target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/patternDetector.h
        ${CMAKE_CURRENT_LIST_DIR}/circlePatternGenerator.h
        ${CMAKE_CURRENT_LIST_DIR}/boardAligner.h

    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/circlePatternGenerator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/boardAligner.cpp
        ${CMAKE_CURRENT_LIST_DIR}/patternDetector.cpp
)
