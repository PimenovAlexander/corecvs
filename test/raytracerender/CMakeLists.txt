project (RaytraceRenderer)

set (NAME raytrace_renderer )

set (SOURCES
    main_raytracerender.cpp  
    scene_large.cpp  
    scene_pole.cpp  
    scene_scanner.cpp  
    scene_speedup.cpp  
    scene_test1.cpp
)



#set (SOURCES ${SOURCES} ${JSON_MODERN_HEADERS} ${JSON_MODERN_SOURCES})

#add_definitions( -DWITH_JSONMODERN )
#include_directories(${JSON_MODERN_INCLUDES})
#message(STATUS FlowDetector:${JSON_MODERN_INCLUDES} )

include(../../wrappers/jsonmodern/sourcelist.cmake)
include(../../wrappers/libpng/sourcelist.cmake)
include(../../wrappers/libjpeg/sourcelist.cmake)

message(STATUS RaytraceRenderer:${INCLUDEPATHS} )

add_executable(${NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

 
