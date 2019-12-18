project (mesh3dplayground)

set (SRC_FILES
    main_mesh3dplayground.cpp
)

set (HDR_FILES
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

set (NAME mesh3dplayground )


add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS})

target_link_libraries(${NAME} ${LIBS} corecvs pthread)

