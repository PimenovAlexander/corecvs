project (Gcodeplayground)


set (GCODE_PLAYGROUND_NAME gcodeplayground )

set (GCODE_PLAYGROUND_SOURCES
    labelGcodeInterpreter.cpp  
    labelGcodeInterpreter.h  
    main_gcodeplayground.cpp  
    vinylCutterInterpreter.cpp  
    vinylCutterInterpreter.h
)

add_executable(${GCODE_PLAYGROUND_NAME} ${GCODE_PLAYGROUND_SOURCES})

target_link_libraries(${GCODE_PLAYGROUND_NAME} corecvs pthread)

