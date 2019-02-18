target_sources(corecvs
  PUBLIC
    geometry/ellipticalApproximation.h 
    geometry/axisAlignedBox.h 
    geometry/rectangle.h 
    geometry/line.h 
    geometry/triangulation.h 
    geometry/polygons.h 
    geometry/mesh3d.h 
    geometry/vptree.h 
    geometry/convexPolyhedron.h 
    geometry/conic.h 
    geometry/polygonPointIterator.h 
#    geometry/projection.h
    geometry/gentryState.h     
    geometry/twoViewOptimalTriangulation.h 
    geometry/mesh3DDecorated.h 
    geometry/polyLine.h 
    geometry/polylinemesh.h 
    geometry/ellipse.h 
    geometry/ellipseFit.h 
    geometry/plane3dFit.h 
    geometry/meshClicker.h
    geometry/convexHull.h
    geometry/convexQuickHull.h
    geometry/projectiveConvexQuickHull.h
    geometry/pointCloud.h
    
    geometry/halfspaceIntersector.h
    geometry/orientedBox.h
    geometry/plane.h
    geometry/kdtree.h
    geometry/beziercurve.h
    geometry/raytrace/bspTree.h

    geometry/ellipticalApproximation.cpp 
    geometry/axisAlignedBox.cpp 
    geometry/rectangle.cpp 
    geometry/triangulation.cpp 
    geometry/polygons.cpp 
    geometry/mesh3d.cpp 
    geometry/convexPolyhedron.cpp 
    geometry/conic.cpp 
    geometry/polygonPointIterator.cpp 
    geometry/projection.cpp 
    geometry/gentryState.cpp   
    geometry/mesh3DDecorated.cpp 
    geometry/polyLine.cpp 
    geometry/polylinemesh.cpp 
    geometry/twoViewOptimalTriangulation.cpp 
    geometry/ellipse.cpp 
    geometry/ellipseFit.cpp 
    geometry/plane3dFit.cpp 
    geometry/meshClicker.cpp
    geometry/convexHull.cpp
    geometry/convexQuickHull.cpp
    geometry/projectiveConvexQuickHull.cpp
    geometry/pointCloud.cpp
    
    geometry/halfspaceIntersector.cpp
    geometry/orientedBox.cpp
    geometry/plane.cpp

)

set(with_renderer YES)

if(${with_renderer})
    target_sources(corecvs
      PUBLIC
        geometry/raytrace/raytraceableNodeWrapper.h
        geometry/raytrace/raytraceRenderer.h
        geometry/raytrace/raytraceObjects.h
        geometry/raytrace/perlinNoise.h
        geometry/raytrace/sdfRenderable.h
        geometry/raytrace/materialExamples.h
        geometry/raytrace/sdfRenderableObjects.h
        geometry/renderer/attributedTriangleSpanIterator.h
        geometry/renderer/simpleRenderer.h
        geometry/renderer/geometryIterator.h


        geometry/raytrace/raytraceRenderer.cpp
        geometry/raytrace/raytraceObjects.cpp
        geometry/raytrace/perlinNoise.cpp
        geometry/raytrace/sdfRenderable.cpp
        geometry/raytrace/materialExamples.cpp
        geometry/raytrace/raytraceableNodeWrapper.cpp
        geometry/renderer/simpleRenderer.cpp
        geometry/renderer/attributedTriangleSpanIterator.cpp
        geometry/raytrace/sdfRenderableObjects.cpp

    )
endif(${with_renderer})




