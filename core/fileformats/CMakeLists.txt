set(FILEFORMATS_HEADER_FILES
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
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/entities/dxfEntity.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/entities/dxfEntityData.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/objects/dxfObject.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/objects/dxfObjectData.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/blocks/dxfBlock.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfDrawing.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfCodes.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfBuilder.h
    )

set(FILEFORMATS_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/bufferLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/bmpLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ppmLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rawLoader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plyLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/stlLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/floLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/openCVDataLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/tgaLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/entities/dxfEntity.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/objects/dxfObject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/blocks/dxfBlock.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfDrawing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfCodes.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfBuilder.cpp
    )

if(1)
 
set(FILEFORMATS_HEADER_FILES
    ${FILEFORMATS_HEADER_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/meshLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/objLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/gcodeLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/pltLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/xyzListLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/svgLoader.h
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfLoader.h
    PARENT_SCOPE
    )
    
set(FILEFORMATS_SOURCE_FILES
    ${FILEFORMATS_SOURCE_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/meshLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/objLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gcodeLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pltLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xyzListLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/svgLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dxf_support/dxfLoader.cpp
    )
endif()

set(FILEFORMATS_HEADER_FILES ${FILEFORMATS_HEADER_FILES} PARENT_SCOPE)
set(FILEFORMATS_SOURCE_FILES ${FILEFORMATS_SOURCE_FILES} PARENT_SCOPE)
