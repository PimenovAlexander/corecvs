project (avencode)

set (MODULE_NAME avencode)

set (SRC_FILES
    main_avencode.cpp
)


if(AVCODEC_LIBS) 
    message("Switching on avcodec support <${AVCODEC_LIBS}>")    
    include(../../wrappers/avcodec/sourcelist.cmake)
endif()

add_executable(${MODULE_NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS})

target_link_libraries(${MODULE_NAME} ${LIBS} stdc++fs corecvs  )

