project (drone-physics-test)


set (MODULE_NAME drone-physics-test )

set (SRC_FILES
    main_physics_test.cpp
)

add_executable(${MODULE_NAME} ${SRC_FILES})

target_link_libraries(${MODULE_NAME} gtest gtest_main drone-core cvs_utils corecvs pthread ${LIBS})
target_include_directories(${MODULE_NAME} PUBLIC ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

