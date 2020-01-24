HEADERS += \
    geometry/ellipticalApproximation.h \
    geometry/axisAlignedBox.h \
    geometry/rectangle.h \
    geometry/line.h \
    geometry/triangulation.h \
    geometry/polygons.h \
    geometry/mesh3d.h \
    geometry/vptree.h \
    geometry/convexPolyhedron.h \
    geometry/conic.h \
    geometry/polygonPointIterator.h \
#    geometry/projection.h \
    geometry/gentryState.h \    
    geometry/twoViewOptimalTriangulation.h \
    geometry/mesh3DDecorated.h \
    geometry/polyLine.h \
    geometry/polylinemesh.h \
    geometry/ellipse.h \
    geometry/ellipseFit.h \
    geometry/plane3dFit.h \
    $$PWD/meshClicker.h \
    $$PWD/convexHull.h \
    $$PWD/convexQuickHull.h \
    $$PWD/projectiveConvexQuickHull.h \
    $$PWD/pointCloud.h \
    \
    $$PWD/halfspaceIntersector.h \
    $$PWD/orientedBox.h \
    $$PWD/plane.h \
    $$PWD/kdtree.h \
    $$PWD/beziercurve.h \
    $$PWD/raytrace/bspTree.h \


SOURCES += \ 
    geometry/ellipticalApproximation.cpp \
    geometry/axisAlignedBox.cpp \
    geometry/rectangle.cpp \
    geometry/triangulation.cpp \
    geometry/polygons.cpp \
    geometry/mesh3d.cpp \
    geometry/convexPolyhedron.cpp \
    geometry/conic.cpp \
    geometry/polygonPointIterator.cpp \
    geometry/gentryState.cpp \  
    geometry/mesh3DDecorated.cpp \
    geometry/polyLine.cpp \
    geometry/polylinemesh.cpp \
    geometry/twoViewOptimalTriangulation.cpp \
    geometry/ellipse.cpp \
    geometry/ellipseFit.cpp \
    geometry/plane3dFit.cpp \
    $$PWD/meshClicker.cpp \
    $$PWD/convexHull.cpp \
    $$PWD/convexQuickHull.cpp \
    $$PWD/projectiveConvexQuickHull.cpp \
    $$PWD/pointCloud.cpp \
    \
    $$PWD/halfspaceIntersector.cpp \
    $$PWD/orientedBox.cpp \
    $$PWD/plane.cpp

CONFIG += with_renderer

with_renderer {

HEADERS += \
    $$PWD/raytrace/raytraceableNodeWrapper.h \
    $$PWD/raytrace/raytraceRenderer.h \
    $$PWD/raytrace/raytraceObjects.h \
    $$PWD/raytrace/perlinNoise.h \
    $$PWD/raytrace/sdfRenderable.h \
    $$PWD/raytrace/materialExamples.h \
    $$PWD/raytrace/sdfRenderableObjects.h \
    $$PWD/renderer/attributedTriangleSpanIterator.h \
    $$PWD/renderer/simpleRenderer.h \
    $$PWD/renderer/geometryIterator.h \

SOURCES += \
    $$PWD/raytrace/raytraceRenderer.cpp \
    $$PWD/raytrace/raytraceObjects.cpp \
    $$PWD/raytrace/perlinNoise.cpp \
    $$PWD/raytrace/sdfRenderable.cpp \
    $$PWD/raytrace/materialExamples.cpp \
    $$PWD/raytrace/raytraceableNodeWrapper.cpp \
    $$PWD/renderer/simpleRenderer.cpp \
    $$PWD/renderer/attributedTriangleSpanIterator.cpp \
    $$PWD/raytrace/sdfRenderableObjects.cpp \

}




OTHER_FILES +=$$PWD/CMakeLists.txt
