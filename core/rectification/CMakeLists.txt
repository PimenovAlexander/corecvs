target_sources(corecvs
    PUBLIC
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
     PRIVATE
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
 )



#SOURCES_NOOPTIMIZE += xml/generated/essentialDerivative.cpp

#with_fastbuild:!win32 {

#    message("Rectification module would use fastbuild")
#    nooptimize.name = nooptimize
#    nooptimize.input = SOURCES_NOOPTIMIZE
#    nooptimize.dependency_type = TYPE_C
#    nooptimize.variable_out = OBJECTS
#    nooptimize.output = ${QMAKE_VAR_OBJECTS_DIR}${QMAKE_FILE_IN_BASE}$${first(QMAKE_EXT_OBJ)}
#    nooptimize.commands = $${QMAKE_CXX} $(CXXFLAGS) -O0 $(INCPATH) -c ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT} # Note the -O0
#    QMAKE_EXTRA_COMPILERS += nooptimize

#} else {
#     $$SOURCES_NOOPTIMIZE
#}


