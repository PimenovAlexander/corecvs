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

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()


add_executable(${FOCUS_STACK_NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS})

target_link_libraries(${FOCUS_STACK_NAME} ${LIBS} stdc++fs corecvs)
