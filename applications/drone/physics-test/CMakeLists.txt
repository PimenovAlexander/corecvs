project (drone-physics-test)


set (NAME drone-physics-test )

set (SRC_FILES
    testPhysicsObject.cpp
    dzhanibekovBolt.cpp
    main_physics_test.cpp
    sim_physics_test.cpp
)


set (HDR_FILES
    testPhysicsObject.h
    dzhanibekovBolt.h
)


if(AVCODEC_LIBS) 
    message("Switching on avcodec support.")
    include(../../../wrappers/avcodec/sourcelist.cmake)
endif()

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES} )

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

if(AVCODEC_LIBS) 
    target_link_libraries(${NAME} ${AVCODEC_LIBS})
endif()

target_link_libraries(${NAME} gtest gtest_main drone-core cvs_utils corecvs pthread ${LIBS})
target_include_directories(${NAME} PUBLIC ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} .)

