project (RaytraceRenderer)

set (NAME raytrace_renderer )

set (SRC_FILES
    main_raytracerender.cpp  
    scene_large.cpp  
    scene_pole.cpp  
    scene_scanner.cpp  
    scene_speedup.cpp  
    scene_test1.cpp
)

include(../../wrappers/jsonmodern/sourcelist.cmake)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

#message(RaytraceRenderer:${INCLUDEPATHS} )

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

include_directories(${INC_PATHS})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
