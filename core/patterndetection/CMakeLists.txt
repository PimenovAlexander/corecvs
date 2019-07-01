target_sources(corecvs
    PUBLIC
        patterndetection/patternDetector.h
        patterndetection/circlePatternGenerator.h
        patterndetection/boardAligner.h

    PRIVATE
        patterndetection/circlePatternGenerator.cpp
        patterndetection/boardAligner.cpp
        patterndetection/patternDetector.cpp
)
