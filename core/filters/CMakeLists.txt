target_sources(corecvs
  PUBLIC
    filters/newstyle/newStyleBlock.h
    filters/newstyle/newStyleBlock.cpp
    filters/newstyle/algoBase.h
)


set (OLDFILTERS_
    filters/legacy/*.h 
    filters/legacy/blocks/*.h 
    filters/legacy/*.cpp 
    filters/legacy/blocks/*.cpp 
    )


#with_oldfilters {
#     $${OLDFILTERS_HEADERS}
#     $${OLDFILTERS_SOURCES}
#} else {
#    OTHER_FILES += $${OLDFILTERS_HEADERS} $${OLDFILTERS_SOURCES}
#}
