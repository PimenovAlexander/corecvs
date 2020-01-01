cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME test)
init_project(PROJECT_NAME raytracerenderer)

set(SOURCE_FILES
    main_raytracerender.cpp  
    scene_large.cpp  
    scene_pole.cpp  
    scene_scanner.cpp  
    scene_speedup.cpp  
    scene_test1.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${SOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

set(ADDITIONAL_LIBS)

if (PNG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        PNGwrapper
        )
endif()

if (JPEG_LIB)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        LIBJPEGwrapper
        )
endif()

target_link_libraries(${PROJECT_NAME}
    pthread
    corecvs
    JSONMODERNwrapper
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )