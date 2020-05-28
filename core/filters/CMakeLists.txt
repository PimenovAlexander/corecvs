set(FILTERS_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/newstyle/newStyleBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/newstyle/algoBase.h
    )

set(FILTERS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/newstyle/newStyleBlock.cpp
    )

SET(OLDFILTERS_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/legacy/abstractFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/backgroundFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/binarizeBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/bitSelectorFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/cannyFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/filterGraph.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/filtersCollection.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/gainOffsetFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/inputFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/maskFilterBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/operationFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/outputFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/sobelFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/thickeningBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/txtFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/compoundFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/filterBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/pins.h
    )

SET(OLDFILTERS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/legacy/abstractFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/backgroundFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/binarizeBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/bitSelectorFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/cannyFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/filtersCollection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/gainOffsetFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/inputFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/maskFilterBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/operationFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/outputFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/sobelFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/thickeningBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/txtFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/compoundFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/filterBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/filterGraph.cpp
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/pins.cpp
    )

set(FILTERS_HEADER_FILES
    ${FILTERS_HEADER_FILES}
#    ${OLDFILTERS_HEADER_FILES}
    )
    
set(FILTERS_SOURCE_FILES
    ${FILTERS_SOURCE_FILES}
#    ${OLDFILTERS_SOURCE_FILES}
    )

set(FILTERS_HEADER_FILES ${FILTERS_HEADER_FILES} PARENT_SCOPE)
set(FILTERS_SOURCE_FILES ${FILTERS_SOURCE_FILES} PARENT_SCOPE)
