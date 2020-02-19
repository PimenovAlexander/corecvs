set(PATTERNDETECTION_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/patternDetector.h
    ${CMAKE_CURRENT_LIST_DIR}/circlePatternGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/boardAligner.h
    ${CMAKE_CURRENT_LIST_DIR}/dummyPatternDetector.h
    ${CMAKE_CURRENT_LIST_DIR}/harrisPatternDetector.h
    PARENT_SCOPE
    )

set(PATTERNDETECTION_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/circlePatternGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/boardAligner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/patternDetector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dummyPatternDetector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/harrisPatternDetector.cpp
    PARENT_SCOPE
    )
