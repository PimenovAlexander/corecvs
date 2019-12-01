project (drone-physics-test)


set (NAME drone-physics-test )

set (SRC_FILES
    main_physics_test.cpp
)

if(AVCODEC_LIBS) 
    message("Switching on avcodec support.")
    include(../../../wrappers/avcodec/sourcelist.cmake)
endif()

add_executable(${NAME} ${SRC_FILES})

if(AVCODEC_LIBS) 
    target_link_libraries(${NAME} ${AVCODEC_LIBS})
endif()

target_link_libraries(${NAME} gtest gtest_main drone-core cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

