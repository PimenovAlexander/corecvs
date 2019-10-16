target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/newstyle/newStyleBlock.h
        ${CMAKE_CURRENT_LIST_DIR}/newstyle/algoBase.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/newstyle/newStyleBlock.cpp
)


set (OLDFILTERS_
    ${CMAKE_CURRENT_LIST_DIR}/legacy/*.h 
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/*.h 
    ${CMAKE_CURRENT_LIST_DIR}/legacy/*.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/legacy/blocks/*.cpp 
    )


#with_oldfilters {
#     $${OLDFILTERS_HEADERS}
#     $${OLDFILTERS_SOURCES}
#} else {
#    OTHER_FILES += $${OLDFILTERS_HEADERS} $${OLDFILTERS_SOURCES}
#}
