cmake_minimum_required (VERSION 3.10)
project (CoreCVS)

set (CoreCVS_VERSION_MAJOR 1)
set (CoreCVS_VERSION_MINOR 0)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules/")

# Overall dependances
option(USE_TBB "Should compile with TBB" YES)
IF ( USE_TBB )
    MESSAGE( STATUS "Including TBB on CORECVS build" )
    find_package(TBB REQUIRED)
    if (TBB_LIBRARY)
        add_definitions(-DWITH_TBB)
    endif()
ENDIF ()


#option(USE_TBB "Should compile with FILESYSTEM" YES)
#IF ( USE_TBB )
#    MESSAGE( STATUS "Including FILESYSTEM on CORECVS build" )
#    find_package(Filesystem)
#    if (FILESYSTEM_LIBRARY)
#        add_definitions(-DWITH_FILESYSTEM)
#    endif()
#ENDIF ()

option(USE_OPENBLAS "Should compile with OpenBlas" YES)
IF ( USE_OPENBLAS )
    MESSAGE( STATUS "Including OpenBlas and Lapacke on CORECVS build" )
    find_package( OpenBlas )
    find_package( Lapacke )
    if (OpenBLAS_LIB AND Lapacke_LIB)
        add_definitions(-DWITH_BLAS)
    endif()
ENDIF ()

option(USE_OPENCV "Should compile with OpenCV" YES)
IF ( USE_OPENCV )
    MESSAGE( STATUS "Including OpenCV on CORECVS build" )
    find_package( OpenCV 3.4 REQUIRED )
    if (OpenCV_LIBS)
        add_definitions(-DWITH_OPENCV)
    else()
        message("You requested OPENCV in the build, but none was found.")
    endif()
ENDIF ()

option(USE_LIBPNG "Should compile with LibPNG" YES)
IF ( USE_LIBPNG )
    MESSAGE( STATUS "Including LibPNG wherever possible" )
    find_package( Png )
ENDIF ()

option(USE_LIBJPEG "Should compile with LibJPEG" YES)
IF ( USE_LIBJPEG )
    MESSAGE( STATUS "Including LibJpeg wherever possible" )
    find_package( Jpeg )
ENDIF ()

option(USE_EIGEN "Should compile with Eigen solver" YES)
IF ( USE_EIGEN )
    MESSAGE( STATUS "Including Eigen solver wherever possible" )
    find_package( Eigen )
ENDIF ()

option(USE_CERES "Should compile with Ceres solver" YES)
IF ( USE_CERES )
    MESSAGE( STATUS "Including Ceres solver wherever possible" )
    find_package( Ceres )
ENDIF ()

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
IF ( USE_AVCODEC )
    MESSAGE( STATUS "Including AVCODEC wherever possible" )
    find_package( AVCodec )    
    IF ( AVCODEC_FOUND )
        MESSAGE( STATUS "Including AVCODEC found" )
    ELSE()
        MESSAGE( STATUS "Including AVCODEC requested, but libaray not found" )
    ENDIF()
ENDIF ()


option(USE_APRILTAG "Should compile with APRILTAG" YES)
IF ( USE_APRILTAG )
    MESSAGE(STATUS "Including Apriltag wherever possible")
    find_package(Apriltag)
    find_package(Threads)
    if (APRILTAG_FOUND)
        add_definitions(-DWITH_APRILTAG)
    else()
        message("You requested USE_APRILTAG in the build, but none was found.")
    endif()
else()
    MESSAGE(STATUS "Apriltag switched off")
endif()

##
#   Additional dependances for web related applications
##

option(USE_AWSSDK "Should compile with AWS SDK" YES)
IF ( USE_AWSSDK )
    option(BUILD_SHARED_LIBS "Build shared libraries" ON)
    find_package(AWSSDK QUIET COMPONENTS s3 cloudtrail)
ENDIF ()

option(USE_POSTGRES "Should compile with POSTGRES" YES)
IF ( USE_POSTGRES )
    find_package(PQXX)
ENDIF ()


###
#
# Debug/Release switch 

#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC")


include(cmake/CMakeCpuOptions.cmake)


###
#
# Actual subprojects
#

include(cmake/googletest.cmake)
fetch_googletest(
    ${PROJECT_SOURCE_DIR}/cmake
    ${PROJECT_BINARY_DIR}/googletest
    )

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(core)
add_subdirectory(utils)

add_subdirectory(test)
add_subdirectory(applications)
add_subdirectory(tools)


enable_testing()
add_subdirectory(test-core)
add_subdirectory(test-core/-perf)




