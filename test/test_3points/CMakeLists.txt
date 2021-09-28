project (test_3point)

set (NAME test_3point)

set (SRC_FILES
    main_3point.cpp
    errorEstimatorBlock.cpp
    gen/error3PointParameters.cpp  
    gen/errorEstimatorBlockBase.cpp  
    gen/vector2dParameters.cpp  
    gen/vector3dParameters.cpp
)

set (HDR_FILES
    errorEstimatorBlock.h
    gen/error3PointParameters.h  
    gen/errorEstimatorBlockBase.h  
    gen/targetType3Point.h  
    gen/vector2dParameters.h  
    gen/vector3dParameters.h
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

include(../../wrappers/jsonmodern/sourcelist.cmake)

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS} gen)


function(cat IN_FILE OUT_FILE)
    file(READ ${IN_FILE} CONTENTS)
    file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

target_link_libraries(${NAME} ${LIBS} corecvs pthread)

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

