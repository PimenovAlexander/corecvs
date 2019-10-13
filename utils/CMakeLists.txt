project(corecvs_utils)
set(MODULE_NAME cvs_utils)

set (UTILS_SUBDIRECTORIES
    # 3d
    camcalc
    corestructs
    distortioncorrector
    fileformats
    filters
    flowcolorers
    framesources
    memoryuse
    opengl
    rectifier
    scripting
    serializer
    statistics
    uis
    visitors
    widgets
)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets)

add_library(${MODULE_NAME} STATIC)

target_link_libraries(${MODULE_NAME} corecvs Qt5::Widgets Qt5::Core Qt5::Gui)

file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/trackPainter.cpp")

foreach(utils_subdirectory ${UTILS_SUBDIRECTORIES})
    message(STATUS "including utils/${utils_subdirectory}")
    include(${utils_subdirectory}/sourcelist.cmake)
    include_directories(${utils_subdirectory})
endforeach(utils_subdirectory)



# message("${HDR_FILES}")

target_sources(${MODULE_NAME}
    PUBLIC
        ${HDR_FILES}
    PRIVATE
        ${SRC_FILES}
)