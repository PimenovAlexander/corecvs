target_sources(corecvs
    PUBLIC
	iterative/minresQLP.h 
	iterative/pcg.h
    PRIVATE
	iterative/minresQLP.cpp 
	iterative/pcg.cpp
)
