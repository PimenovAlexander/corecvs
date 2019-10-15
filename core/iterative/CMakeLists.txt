target_sources(corecvs
    PUBLIC
		${CMAKE_CURRENT_LIST_DIR}/minresQLP.h 
		${CMAKE_CURRENT_LIST_DIR}/pcg.h
    PRIVATE
		${CMAKE_CURRENT_LIST_DIR}/minresQLP.cpp 
		${CMAKE_CURRENT_LIST_DIR}/pcg.cpp
)
