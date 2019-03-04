target_sources(corecvs
  PUBLIC
    fileformats/bufferLoader.h
    fileformats/bmpLoader.h
    fileformats/ppmLoader.h
    fileformats/rawLoader.h
    fileformats/plyLoader.h
    fileformats/stlLoader.h
    fileformats/metamap.h
    fileformats/floLoader.h
    fileformats/openCVDataLoader.h
    fileformats/tgaLoader.h

 
    fileformats/bufferLoader.cpp
    fileformats/bmpLoader.cpp
    fileformats/ppmLoader.cpp
    fileformats/rawLoader.cpp
    fileformats/plyLoader.cpp
    fileformats/stlLoader.cpp
    fileformats/floLoader.cpp
    fileformats/openCVDataLoader.cpp
    fileformats/tgaLoader.cpp
)

if (YES)
 
    target_sources(corecvs
      PUBLIC
        fileformats/meshLoader.h
        fileformats/objLoader.h
        fileformats/gcodeLoader.h
        fileformats/pltLoader.h
        fileformats/xyzListLoader.h
        fileformats/meshLoader.cpp
        fileformats/objLoader.cpp
        fileformats/gcodeLoader.cpp
        fileformats/pltLoader.cpp
        fileformats/xyzListLoader.cpp
        fileformats/svgLoader.h
        fileformats/svgLoader.cpp
    )
endif()
