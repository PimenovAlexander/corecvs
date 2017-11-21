HEADERS += \
    rectification/correspondenceList.h \
    rectification/essentialMatrix.h \
    rectification/essentialEstimator.h \
    rectification/iterativeEstimator.h \
    rectification/ransacEstimator.h \
    rectification/stereoAligner.h \ 
    rectification/triangulator.h \
    rectification/ransac.h \
    rectification/multicameraTriangulator.h \


SOURCES += \
    rectification/essentialMatrix.cpp \
    rectification/essentialEstimator.cpp \
    rectification/iterativeEstimator.cpp \
    rectification/ransacEstimator.cpp \
    rectification/correspondenceList.cpp \
    rectification/stereoAligner.cpp \
    rectification/triangulator.cpp \
    rectification/multicameraTriangulator.cpp \



SOURCES += \
    xml/generated/essentialDerivative.cpp \
    xml/generated/essentialDerivative1.cpp \
    xml/generated/essentialDerivative2.cpp \