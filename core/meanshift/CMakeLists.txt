target_sources(corecvs
    PUBLIC
        meanshift/meanShiftCalculator.h
        meanshift/meanShiftWindow.h
        meanshift/abstractMeanShiftKernel.h

     PRIVATE
        meanshift/meanShiftCalculator.cpp
        meanshift/meanShiftWindow.cpp
 )

