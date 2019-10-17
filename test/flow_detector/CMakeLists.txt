project (FlowDetector)


set (NAME flow_detector )

set (SOURCES
    main_flow_detector.cpp
)

#set (SOURCES ${SOURCES} ${JSON_MODERN_HEADERS} ${JSON_MODERN_SOURCES})

#add_definitions( -DWITH_JSONMODERN )
#include_directories(${JSON_MODERN_INCLUDES})
#message(STATUS FlowDetector:${JSON_MODERN_INCLUDES} )

include(../../wrappers/jsonmodern/CMakeLists.txt)
include(../../wrappers/libpng/CMakeLists.txt)
include(../../wrappers/libjpeg/CMakeLists.txt)

message(STATUS FlowDetector:${INCLUDEPATHS} )

add_executable(${NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${NAME} corecvs pthread ${LIBS})

