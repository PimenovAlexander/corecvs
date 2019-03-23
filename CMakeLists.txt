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


add_subdirectory(core)
add_subdirectory(test-core)

add_dependencies(test-core corecvs)

#add_executable(Tutorial tutorial.cxx)
