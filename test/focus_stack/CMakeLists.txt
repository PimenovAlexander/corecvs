project (FocusStack)

set (FOCUS_STACK_NAME focus_stack )

set (FOCUS_STACK_SOURCES
    imageStack.cpp 
    laplacianStacking.cpp 
    main.cpp
)

set (FOCUS_STACK_HEADERS
    FSAlgorithm.h 
    imageStack.h 
    laplacianStacking.h
)

add_executable(${FOCUS_STACK_NAME} ${FOCUS_STACK_SOURCES})

target_link_libraries(${FOCUS_STACK_NAME} stdc++fs corecvs)
