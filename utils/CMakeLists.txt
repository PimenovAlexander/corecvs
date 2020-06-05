cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME cvs_utils)
init_project(PROJECT_NAME corecvs_utils)

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script Xml SerialPort)

set(UTILS_MODULES
    camcalc
    corestructs          
    distortioncorrector
    fileformats
    filters              
    flowcolorers         
    framesources
    #memoryuse     
    rectifier
    scripting
    serializer
    statistics
    uis
    visitors
    widgets
    )
    
set(UTILS_OPENGL_MODULES
    3d
    opengl
    )

set(PUBLIC_HEADER_FILES
    configManager.h
    flowDrawer.h
    matrixwidget.h
    qtHelper.h
    scannercontrol.h
    timeliner.h
    trackPainter.h
    viAreaWidget.h
    viGLAreaWidget.h
    )

foreach(utils_module ${UTILS_MODULES})
    message(STATUS "including ${utils_module}")
    add_subdirectory(${utils_module})
    string(TOUPPER ${utils_module} utils_module)
    set(HEADERS
        ${HEADERS}
        ${${utils_module}_HEADER_FILES}
        )
    set(SOURCES
        ${SOURCES}
        ${${utils_module}_SOURCE_FILES}
        )
endforeach(utils_module)

option(WITH_OPENGL "Should use OpenGL from Qt" YES)

if(WITH_OPENGL)
    foreach(utils_opengl_module ${UTILS_OPENGL_MODULES})
        message(STATUS "including ${utils_opengl_module}")
        add_subdirectory(${utils_opengl_module})
        string(TOUPPER ${utils_opengl_module} utils_opengl_module)
        set(HEADERS
            ${HEADERS}
            ${${utils_opengl_module}_HEADER_FILES}
            )
        set(SOURCES
            ${SOURCES}
            ${${utils_opengl_module}_SOURCE_FILES}
            )
    endforeach(utils_opengl_module)
endif()

set(HEADERS
    ${HEADERS}
    ${PUBLIC_HEADER_FILES}
    ${UIS_CLOUDVIEW_HEADER_FILES}
    )

set(SOURCE_FILES
    configManager.cpp
    flowDrawer.cpp
    matrixwidget.cpp
    qtHelper.cpp
    scannercontrol.cpp
    timeliner.cpp
    viAreaWidget.cpp
    viGLAreaWidget.cpp
    )

set(SOURCES
    ${SOURCES}
    ${SOURCE_FILES}
    ${UIS_CLOUDVIEW_SOURCE_FILES}
    )

set(XML_RESOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/../tools/generator/xml/utils.xml
    ${CMAKE_CURRENT_LIST_DIR}/../tools/generator/xml/draw3dutils.xml
    ${CMAKE_CURRENT_LIST_DIR}/../tools/generator/xml/graphPlot.xml
    )

set_source_files_properties(${XML_RESOURCE_FILES} 
    PROPERTIES 
    EXTERNAL_OBJECT TRUE 
    HEADER_FILE_ONLY TRUE
    )

set(RESOURCES
    ${XML_RESOURCE_FILES}
    ../resources/main.qrc
    )

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_library(${PROJECT_NAME} STATIC
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

set(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_autogen/include")

if(WIN32)
    if(USE_OPENCV)
        set(OPENCV_MODULES
            $ENV{OPENCV_DIR}/
            $ENV{OPENCV_DIR}/../modules/core/include
            $ENV{OPENCV_DIR}/../modules/highgui/include
            $ENV{OPENCV_DIR}/../modules/imgcodecs/include
            $ENV{OPENCV_DIR}/../modules/imgproc/include
            $ENV{OPENCV_DIR}/../modules/objdetect/include
            $ENV{OPENCV_DIR}/../modules/videoio/include
            $ENV{OPENCV_DIR}/../modules/videostab/include
            $ENV{OPENCV_DIR}/../modules/videostab/include
            $ENV{OPENCV_CONTRIB_DIR}/../modules/optflow/include
            )
    endif()
endif(WIN32)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${AUTOGEN_BUILD_DIR} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OPENCV_MODULES}
    )

set(ADDITIONAL_LIBS)

if (OpenCV_LIBS)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        OPENCVwrapper
    )
endif()

if (APRILTAG_FOUND)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        Threads::Threads
        APRILTAGwrapper
        )
endif()

if(WITH_OPENGL)
    find_package(Qt5 COMPONENTS REQUIRED OpenGL)
    set(ADDITIONAL_LIBS
        ${ADDITIONAL_LIBS}
        Qt5::OpenGL
        GLU
        GL
        )
endif()

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corecvs 
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Script
    Qt5::Xml
    Qt5::SerialPort
    JSONMODERNwrapper
    LIBPNGwrapper
    LIBJPEGwrapper
    V4L2wrapper
    AVCODECwrapper
    ${ADDITIONAL_LIBS}
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    AUTOMOC "TRUE"
    AUTOUIC "TRUE"
    AUTORCC "TRUE"
    FOLDER "${MODULE_NAME}"
    )