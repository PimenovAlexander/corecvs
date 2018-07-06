HEADERS += \
    rectification/correspondenceList.h \
    rectification/essentialMatrix.h \
    rectification/essentialEstimator.h \
    rectification/iterativeEstimator.h \
    rectification/ransacEstimator.h \
    rectification/stereoAligner.h \ 
    rectification/triangulator.h \
    rectification/ransac.h \
#    rectification/multicameraEstimator.h \
    rectification/multicameraTriangulator.h \
    $$PWD/sceneStereoAlignerBlock.h


SOURCES += \
    rectification/essentialMatrix.cpp \
    rectification/essentialEstimator.cpp \
    rectification/iterativeEstimator.cpp \
    rectification/ransacEstimator.cpp \
    rectification/correspondenceList.cpp \
    rectification/stereoAligner.cpp \
    rectification/triangulator.cpp \
#    rectification/multicameraEstimator.cpp \
    rectification/multicameraTriangulator.cpp \
    $$PWD/sceneStereoAlignerBlock.cpp

SOURCES += \    
    xml/generated/essentialDerivative1.cpp \
    xml/generated/essentialDerivative2.cpp \

SOURCES_NOOPTIMIZE += xml/generated/essentialDerivative.cpp

with_fastbuild:!win32 {

    message("Rectification module would use fastbuild")
    nooptimize.name = nooptimize
    nooptimize.input = SOURCES_NOOPTIMIZE
    nooptimize.dependency_type = TYPE_C
    nooptimize.variable_out = OBJECTS
    nooptimize.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
    nooptimize.commands = $${QMAKE_CXX} $(CXXFLAGS) -O0 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} # Note the -O0
    QMAKE_EXTRA_COMPILERS += nooptimize


} else {

    SOURCES += $$SOURCES_NOOPTIMIZE
}


