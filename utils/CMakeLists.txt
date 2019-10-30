project(corecvs_utils)
set(MODULE_NAME cvs_utils)

# include(../wrappers/jsonmodern/CMakeLists.txt)
# include(../wrappers/libpng/CMakeLists.txt)
# include(../wrappers/libjpeg/CMakeLists.txt)
# include(../wrappers/v4l2/CMakeLists.txt)


set (UTILS_SUBDIRECTORIES
    camcalc
    corestructs
    distortioncorrector
    fileformats
#    filters
    flowcolorers
    framesources
    # memoryuse
    # opengl
    rectifier
    scripting
    serializer
    statistics
    uis
    visitors
    widgets   
    ../wrappers/jsonmodern
    ../wrappers/libpng
    ../wrappers/libjpeg
    ../wrappers/v4l2
)

if (OpenCV_LIBS)
    set(UTILS_SUBDIRECTORIES
        ${UTILS_SUBDIRECTORIES}
        ../wrappers/opencv
    )
endif()


option(WITH_OPENGL "Should use OpenGL from Qt" YES)
if(WITH_OPENGL)
    set(UTILS_SUBDIRECTORIES
        ${UTILS_SUBDIRECTORIES}
        3d
        opengl
        uis/cloudview
    )
endif()


set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")
set(CMAKE_AUTORCC "YES")


find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script Xml)

add_library(${MODULE_NAME} STATIC)

target_link_libraries(${MODULE_NAME} corecvs Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Script Qt5::Xml)

if(WITH_OPENGL)
    find_package(Qt5 COMPONENTS REQUIRED OpenGL)
    target_link_libraries(${MODULE_NAME} Qt5::OpenGL GLU GL)
endif()


file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
list(REMOVE_ITEM SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/trackPainter.cpp")

foreach(utils_subdirectory ${UTILS_SUBDIRECTORIES})
    message(STATUS "including utils/${utils_subdirectory}")
    target_include_directories(${MODULE_NAME} PUBLIC ${utils_subdirectory})
    include(${utils_subdirectory}/sourcelist.cmake)
endforeach(utils_subdirectory)



message("Additional include path <${INC_PATHS}>")
target_include_directories(${MODULE_NAME} PUBLIC ${INC_PATHS})
target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# Temporary fixes 
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${MODULE_NAME}_autogen/include")
target_include_directories(${MODULE_NAME} PUBLIC ${AUTOGEN_BUILD_DIR} )
message("AUTOGEN_BUILD_DIR bin directory <${AUTOGEN_BUILD_DIR}>")


target_sources(${MODULE_NAME}
    PUBLIC
        ${HDR_FILES}
    PRIVATE
        ${SRC_FILES}
        ../resources/main.qrc
)

