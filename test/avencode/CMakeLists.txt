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


if(AVCODEC_LIBS) 
    target_link_libraries(${MODULE_NAME} ${AVCODEC_LIBS})
endif()

target_link_libraries(${MODULE_NAME} stdc++fs corecvs  )

