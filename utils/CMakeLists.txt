project(corecvs_utils)
set(MODULE_NAME cvs_utils)

include(../wrappers/jsonmodern/CMakeLists.txt)
include(../wrappers/libpng/CMakeLists.txt)
include(../wrappers/libjpeg/CMakeLists.txt)
include(../wrappers/v4l2/CMakeLists.txt)


set (UTILS_SUBDIRECTORIES
    camcalc
    corestructs
    distortioncorrector
    fileformats
    filters
    flowcolorers
    # framesources
    # memoryuse
    # opengl
    rectifier
    scripting
    serializer
    statistics
    uis
    visitors
    widgets
)

if(opengl)
    set(UTILS_SUBDIRECTORIES
        ${UTILS_SUBDIRECTORIES}
        3d

    )
endif()


set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

add_library(${MODULE_NAME} STATIC)

target_link_libraries(${MODULE_NAME} corecvs Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Script)

file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/trackPainter.cpp")

foreach(utils_subdirectory ${UTILS_SUBDIRECTORIES})
    message(STATUS "including utils/${utils_subdirectory}")
    include(${utils_subdirectory}/sourcelist.cmake)
    include_directories(${utils_subdirectory})
endforeach(utils_subdirectory)



target_sources(${MODULE_NAME}
    PUBLIC
        ${HDR_FILES}
    PRIVATE
        ${SRC_FILES}
)

message("Debug:" "${cvs_utils_SOURCE_DIR}")
