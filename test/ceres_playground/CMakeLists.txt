project (ceres_playground)


set (NAME ceres_playground)

set (SRC_FILES
    main_ceres_playground.cpp
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS})

target_link_libraries(${NAME} ${LIBS} stdc++fs corecvs)

