cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME tools)
init_project(PROJECT_NAME generator)

find_package(Qt5 COMPONENTS REQUIRED Core Xml)

set(PRIVATE_HEADER_FILES
    reflectionGenerator.h
    pdoGenerator.h
    widgetUIGenerator.h
    documentationGenerator.h
    baseGenerator.h
    parametersMapperGenerator.h
    configLoader.h
    )

set(HEADERS
    ${PRIVATE_HEADER_FILES}
    )

set(SOURCE_FILES
    main.cpp
    pdoGenerator.cpp
    widgetUIGenerator.cpp
    documentationGenerator.cpp
    baseGenerator.cpp
    parametersMapperGenerator.cpp
    configLoader.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

set(XML_FILES
    xml/base.xml
    xml/baseStub.xml
    xml/copter.xml
    xml/draw3dutils.xml
    xml/egomotion.xml
    xml/graphPlot.xml
    xml/opencv.xml
    xml/opencvsgm.xml
    xml/presentation.xml
    xml/recorder.xml
    xml/rectify.xml
    xml/test.xml
    xml/utils.xml
    )

set(TOOLS_GENERATOR_FILES
    copy-base.sh
    helper-regen.sh
    h_stub.sh
    regen-all.sh
    regen-apriltag.sh
    regen-basestub.sh
    regen-copter.sh
    regen-core.sh
    regen-documentation.sh
    regen-egomotion.sh
    regen-merger.sh
    regen-opencv.sh
    regen-physics.sh
    regen-recorder.sh
    regen-scanner.sh
    regen-test-core.sh
    regen-utils.sh
    selftest.sh
    )

set(RESOURCES
    ${XML_FILES}
    ${TOOLS_GENERATOR_FILES}
    )

set_source_files_properties(${RESOURCES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)

assign_source_group(${HEADERS} ${SOURCES} ${RESOURCES})

add_executable(${PROJECT_NAME}
    ${HEADERS}
    ${SOURCES}
    ${RESOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    Qt5::Core
    Qt5::Xml
    )

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    FOLDER "${MODULE_NAME}"
    )

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${MODULE_NAME}/${PROJECT_NAME}
    )