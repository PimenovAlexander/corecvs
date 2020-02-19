set(RECTIFICATION_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/essentialMatrix.h
    ${CMAKE_CURRENT_LIST_DIR}/essentialEstimator.h
    ${CMAKE_CURRENT_LIST_DIR}/iterativeEstimator.h
    ${CMAKE_CURRENT_LIST_DIR}/ransacEstimator.h
    ${CMAKE_CURRENT_LIST_DIR}/stereoAligner.h
    ${CMAKE_CURRENT_LIST_DIR}/triangulator.h
    ${CMAKE_CURRENT_LIST_DIR}/ransac.h
#    ${CMAKE_CURRENT_LIST_DIR}/multicameraEstimator.h
    ${CMAKE_CURRENT_LIST_DIR}/multicameraTriangulator.h
    ${CMAKE_CURRENT_LIST_DIR}/sceneStereoAlignerBlock.h
    PARENT_SCOPE
    )

set(RECTIFICATION_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/essentialMatrix.cpp
    ${CMAKE_CURRENT_LIST_DIR}/essentialEstimator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/iterativeEstimator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ransacEstimator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stereoAligner.cpp
    ${CMAKE_CURRENT_LIST_DIR}/triangulator.cpp
#    ${CMAKE_CURRENT_LIST_DIR}/multicameraEstimator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/multicameraTriangulator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/sceneStereoAlignerBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../xml/generated/essentialDerivative1.cpp
    ${CMAKE_CURRENT_LIST_DIR}/../xml/generated/essentialDerivative2.cpp
    PARENT_SCOPE
    )