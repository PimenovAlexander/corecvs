target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/bufferLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/bmpLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/ppmLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/rawLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/plyLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/stlLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/metamap.h
        ${CMAKE_CURRENT_LIST_DIR}/floLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/openCVDataLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/tgaLoader.h

     PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/bufferLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/bmpLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/ppmLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/rawLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/plyLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/stlLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/floLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/openCVDataLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tgaLoader.cpp
)

if (1)
 
    target_sources(corecvs
      PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/meshLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/objLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/gcodeLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/pltLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/xyzListLoader.h
        ${CMAKE_CURRENT_LIST_DIR}/svgLoader.h
        
      PRIVATE  
        ${CMAKE_CURRENT_LIST_DIR}/meshLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/objLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/gcodeLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/pltLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/xyzListLoader.cpp
        ${CMAKE_CURRENT_LIST_DIR}/svgLoader.cpp
    )
endif()
