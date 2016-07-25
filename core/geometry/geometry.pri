HEADERS += \
    geometry/ellipticalApproximation.h \
    geometry/axisAlignedBox.h \
    geometry/rectangle.h \
    geometry/line.h \
    geometry/triangulation.h \
    geometry/polygons.h \
    geometry/mesh3d.h \
    geometry/convexPolyhedron.h \
    geometry/renderer/simpleRenderer.h \
    geometry/renderer/geometryIterator.h \
    geometry/conic.h \
    geometry/polygonPointIterator.h \
    geometry/Projection.h \
    geometry/gentryState.h \
    geometry/raytrace/raytraceRenderer.h \
    geometry/raytrace/raytraceObjects.h \
    geometry/mesh3DDecorated.h \
    geometry/polyLine.h \
    geometry/polylinemesh.h \
    $$PWD/raytrace/perlinNoise.h \
    $$PWD/raytrace/sdfRenderable.h \
    $$PWD/raytrace/materialExamples.h


SOURCES += \ 
    geometry/ellipticalApproximation.cpp \
    geometry/axisAlignedBox.cpp \
    geometry/rectangle.cpp \
    geometry/triangulation.cpp \
    geometry/polygons.cpp \
    geometry/mesh3d.cpp \
    geometry/convexPolyhedron.cpp \
    geometry/renderer/simpleRenderer.cpp \
    geometry/conic.cpp \
    geometry/polygonPointIterator.cpp \
    geometry/projection.cpp \
    geometry/gentryState.cpp \
    geometry/raytrace/raytraceRenderer.cpp \
    geometry/raytrace/raytraceObjects.cpp \
    geometry/mesh3DDecorated.cpp \
    geometry/polyLine.cpp \
    geometry/polylinemesh.cpp \
    $$PWD/raytrace/perlinNoise.cpp \
    $$PWD/raytrace/sdfRenderable.cpp \
    $$PWD/raytrace/materialExamples.cpp
    
