project (FocusStack)

set (FOCUS_STACK_NAME focus_stack )

set (SRC_FILES
    imageStack.cpp 
    laplacianStacking.cpp 
    main.cpp
)

set (HDR_FILES
    FSAlgorithm.h 
    imageStack.h 
    laplacianStacking.h
)

add_executable(${FOCUS_STACK_NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${FOCUS_STACK_NAME} stdc++fs corecvs)
