cmake_minimum_required (VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(CoreCVS)

set(CoreCVS_VERSION_MAJOR 1)
set(CoreCVS_VERSION_MINOR 0)

# Debug/Release switch 

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

include(cmake/CMakeCpuOptions.cmake)
include(cmake/functions.cmake)
include(cmake/utility.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Overall dependances
option(USE_TBB "Should compile with TBB" YES)
if(USE_TBB)
    message(STATUS "Including TBB on CORECVS build")
    find_package(TBB REQUIRED)
    if (TBB_LIBRARY)
        add_definitions(-DWITH_TBB)
    endif()
endif()

#option(USE_TBB "Should compile with FILESYSTEM" YES)
#if (USE_TBB)
#    message(STATUS "Including FILESYSTEM on CORECVS build")
#    find_package(Filesystem)
#    if (FILESYSTEM_LIBRARY)
#        add_definitions(-DWITH_FILESYSTEM)
#    endif()
#endif()

option(USE_OPENBLAS "Should compile with OpenBlas" YES)
if(USE_OPENBLAS)
    message(STATUS "Including OpenBlas and Lapacke on CORECVS build")
    find_package(OpenBlas)
    find_package(Lapacke)
    if (OpenBLAS_LIB AND Lapacke_LIB)
        add_definitions(-DWITH_BLAS)
    endif()
endif()

option(USE_OPENCV "Should compile with OpenCV" YES)
if(USE_OPENCV)
    message(STATUS "Including OpenCV on CORECVS build")
    find_package(OpenCV)
    if(OpenCV_LIBS)
        add_definitions(-DWITH_OPENCV)
    else()
        message("You requested OPENCV in the build, but none was found.")
    endif()
endif()

option(USE_LIBPNG "Should compile with LibPNG" YES)
if(USE_LIBPNG)
    message(STATUS "Including LibPNG wherever possible")
    find_package(Png)
endif()

option(USE_LIBJPEG "Should compile with LibJPEG" YES)
if(USE_LIBJPEG)
    message(STATUS "Including LibJpeg wherever possible")
    find_package(Jpeg)
endif()

option(USE_EIGEN "Should compile with Eigen solver" YES)
if(USE_EIGEN)
    message(STATUS "Including Eigen solver wherever possible")
    find_package(Eigen)
endif()

option(USE_CERES "Should compile with Ceres solver" YES)
if(USE_CERES)
    message(STATUS "Including Ceres solver wherever possible")
    find_package(Ceres)
endif()

option(USE_LIBFFT "Should compile with libfftw" YES)
IF ( USE_LIBFFT )
    MESSAGE( STATUS "Including libfftw wherever possible" )
    find_package( FFTW )
ENDIF ()

option(USE_SOAPYSDR "Should compile with SoapySDR" YES)
IF ( USE_SOAPYSDR )
    MESSAGE( STATUS "Including SoapySDR wherever possible" )
    find_package( SoapySDR )
ENDIF ()

option(USE_AVCODEC "Should compile with AVCODEC" YES)
if(USE_AVCODEC)
    message(STATUS "Including AVCODEC wherever possible")
    find_package(AVCodec)
else()
    message(STATUS "Including AVCODEC requested, but libaray not found")
endif()

option(USE_APRILTAG "Should compile with APRILTAG" YES)
if(USE_APRILTAG)
    message(STATUS "Including Apriltag wherever possible")
    find_package(Apriltag)
    find_package(Threads)
    if (APRILTAG_FOUND)
        add_definitions(-DWITH_APRILTAG)
    else()
        message("You requested USE_APRILTAG in the build, but none was found.")
    endif()
else()
    message(STATUS "Apriltag switched off")
endif()

# Actual subprojects

if(UNIX)
    include(cmake/googletest.cmake)
    fetch_googletest(
        ${PROJECT_SOURCE_DIR}/cmake
        ${PROJECT_BINARY_DIR}/googletest
        )
endif(UNIX)

add_subdirectory(core)
add_subdirectory(utils)

if(UNIX)
    add_subdirectory(test)
    add_subdirectory(applications)
    add_subdirectory(tools)
    add_subdirectory(wrappers)

    enable_testing()
    add_subdirectory(test-core)
    add_subdirectory(test-core/-perf)
endif(UNIX)