cmake_minimum_required (VERSION 3.10)
project (CoreCVS)

set (CoreCVS_VERSION_MAJOR 1)
set (CoreCVS_VERSION_MINOR 0)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# Overall dependances
set (USE_TBB 1)

IF ( USE_TBB )
    MESSAGE( STATUS "Including TBB on CORECVS build" )
    find_package( TBB )
ENDIF ()


set (USE_OPENBLAS 1)
IF ( USE_OPENBLAS )
    MESSAGE( STATUS "Including OpenBlas on CORECVS build" )
    find_package( OpenBlas )
ENDIF ()

###############################################################
#
# Options area 
#
# -mavx -mavx2 -mfma -msse2 -msse3 -mssse3 -msse4.1 -msse4.2
#
#
# TODO: 
#  1. support debug and release
#  2. support NEON 
#
#
#

option(WITH_SSE "Should compile with SSE" YES)
if (WITH_SSE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse2")   
    add_definitions( -DWITH_SSE )
endif()

option(WITH_SSE3 "Should compile with SSE3" YES)
if (WITH_SSE3)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse3")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse3")
    add_definitions( -DWITH_SSE3 )
endif()

option(WITH_SSE4_1 "Should compile with SSE4_1" YES)
if (WITH_SSE4_1)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.1")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse4.1")
    add_definitions( -DWITH_SSE4_1 )
endif()

option(WITH_SSE4_2 "Should compile with SSE4_2" YES)
if (WITH_SSE4_2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -msse4.2")
    add_definitions( -DWITH_SSE4_2 )
endif()

option(WITH_AVX "Should compile with AVX" YES)
if (WITH_AVX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -mavx")   
    add_definitions( -DWITH_AVX )
endif()

option(WITH_AVX2 "Should compile with AVX2" YES)
if (WITH_AVX2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -mavx2")   
    add_definitions( -DWITH_AVX2 )
endif()

option(WITH_FMA "Should compile with FMA" YES)
if (WITH_FMA)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mfma")
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -mfma")   
    add_definitions( -DWITH_FMA )
endif()



###
#
# Actual subprojects
#

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
    )

add_subdirectory(core)
add_subdirectory(utils)

#TODO: Bring this to subproject
#add_subdirectory(test)
#add_subdirectory(test/adoptcolor)    # depends on utils
#add_subdirectory(test/avencode)      # includes files from on avencode
#add_subdirectory(test/example_scene) # includes files from on avencode
add_subdirectory(test/fileloader)
add_subdirectory(test/flow_detector)  # includes json
add_subdirectory(test/flowtest)       
add_subdirectory(test/focus_stack)
add_subdirectory(test/gcodeplayground) #??
#add_subdirectory(test/grab24) # ??
#add_subdirectory(test/grab24_qt)      # depends on utils
#add_subdirectory(test/jitplayground) 
add_subdirectory(test/raytracerender) 
add_subdirectory(test/pattern_detector) 
add_subdirectory(test/softrender)


#add_subdirectory(applications)


enable_testing()
add_subdirectory(test-core)
add_subdirectory(test-core/-perf)

