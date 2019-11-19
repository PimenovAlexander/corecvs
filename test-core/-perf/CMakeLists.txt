project (CoreCVSTestsPerf)

set(MODULE_NAME corecvs)
set(MODULE_NAME_TEST corecvs_core_perf_tests)


MESSAGE( STATUS "Including GTest on Tests build" )
#include_directories (${GTest_INCLUDE_DIR}/..)
include_directories (${GTest_INCLUDE_DIR})

set (CORE_TEST_PERF_SOURCES
    main.cpp 
    
    fastkernel_double/main_test_fastkernel_double.cpp 
    fastkernel_profile/main_test_fastkernel_profile.cpp 
    
    matrix_profile/main_test_matrix_profile.cpp 
    
    hamilton/main_test_hamilton_profile.cpp 
    
    ../deform/test_deform.cpp 
    deform_profile/main_test_deform_profile.cpp     
)

message(STATUS "SOURCE_DIR " ${corecvs_SOURCE_DIR})
message(STATUS "TEST_SOURCE_DIR " ${PROJECT_SOURCE_DIR})


add_executable(${MODULE_NAME_TEST} ${CORE_TEST_PERF_SOURCES})

set_property(TARGET ${MODULE_NAME_TEST} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${MODULE_NAME_TEST} PROPERTY CXX_STANDARD_REQUIRED ON)


target_link_libraries(${MODULE_NAME_TEST} gtest gtest_main stdc++fs corecvs)
#target_include_directories(${MODULE_NAME_TEST} PUBLIC ${corecvs_SOURCE_DIR} .)
target_include_directories(${MODULE_NAME_TEST} PUBLIC ${corecvs_SOURCE_DIR})

# For ctest support. Not necessary to use but nice move for automated testing.
add_test(
        NAME
            core-test-perf
        COMMAND
            ./${MODULE_NAME_TEST}
)

