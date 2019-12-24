project (Gcodeplayground)


set (GCODE_PLAYGROUND_NAME gcodeplayground )

set (SRC_FILES
    labelGcodeInterpreter.cpp  
    main_gcodeplayground.cpp  
)

set (HDR_FILES
    labelGcodeInterpreter.h  
)

add_executable(${GCODE_PLAYGROUND_NAME} ${SRC_FILES} ${HDR_FILES})

target_link_libraries(${GCODE_PLAYGROUND_NAME} corecvs pthread)

