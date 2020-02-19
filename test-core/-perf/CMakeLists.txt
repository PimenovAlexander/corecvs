cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME corecvs)
init_project(PROJECT_NAME ${MODULE_NAME}_core_perf_tests)

message(STATUS "Including GTest on Tests build" )

set(DIR_NAME
    test-core/-perf
    )

set(SOURCE_FILES
    main.cpp
    ../deform/test_deform.cpp
    )

set(DEFORM_PROFILE_SOURCE_FILE
    deform_profile/main_test_deform_profile.cpp 
    )

set(FASTKERNEL_DOUBLE_SOURCE_FILE
    fastkernel_double/main_test_fastkernel_double.cpp 
    )

set(HAMILTON_SOURCE_FILE
    hamilton/main_test_hamilton_profile.cpp
    )

set(MATRIX_PROFILE_SOURCE_FILE
    matrix_profile/main_test_matrix_profile.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    ${DEFORM_PROFILE_SOURCE_FILE}
    ${FASTKERNEL_DOUBLE_SOURCE_FILE}
    ${HAMILTON_SOURCE_FILE}
    ${MATRIX_PROFILE_SOURCE_FILE}
    )

assign_source_group(${SOURCES})

message(STATUS "SOURCE_DIR " ${corecvs_SOURCE_DIR})
message(STATUS "TEST_SOURCE_DIR " ${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    )

target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${GTest_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    corecvs
    gtest
    gtest_main
    stdc++fs
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${DIR_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )

# For ctest support. Not necessary to use but nice move for automated testing.
add_test(
    NAME        core-test-perf
    COMMAND     ./${PROJECT_NAME}
    )