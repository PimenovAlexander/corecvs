project (Gcodeplayground)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()


set (NAME gcodeplayground )

set (SRC_FILES ${SRC_FILES}
    labelGcodeInterpreter.cpp  
    main_gcodeplayground.cpp  
    vinylCutterInterpreter.cpp  
)

set (HDR_FILES ${HDR_FILES}
    labelGcodeInterpreter.h  
    vinylCutterInterpreter.h
)

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

include_directories(${INC_PATHS})

target_link_libraries(${NAME} ${LIBS} corecvs pthread)

