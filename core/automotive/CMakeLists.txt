set(AUTOMOTIVE_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/FCostFunction.h
	${CMAKE_CURRENT_LIST_DIR}/flowVectorInformation.h
	${CMAKE_CURRENT_LIST_DIR}/simulation/testSceneSimulator.h
    PARENT_SCOPE
	)

set(AUTOMOTIVE_SOURCE_FILES
	${CMAKE_CURRENT_LIST_DIR}/FCostFunction.cpp
	${CMAKE_CURRENT_LIST_DIR}/flowVectorInformation.cpp
	${CMAKE_CURRENT_LIST_DIR}/simulation/testSceneSimulator.cpp
    PARENT_SCOPE
	)