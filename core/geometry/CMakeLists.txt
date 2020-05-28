set(GEOMETRY_HEADER_FILES
    ${CMAKE_CURRENT_LIST_DIR}/ellipticalApproximation.h
    ${CMAKE_CURRENT_LIST_DIR}/axisAlignedBox.h
    ${CMAKE_CURRENT_LIST_DIR}/rectangle.h
    ${CMAKE_CURRENT_LIST_DIR}/line.h
    ${CMAKE_CURRENT_LIST_DIR}/triangulation.h
    ${CMAKE_CURRENT_LIST_DIR}/polygons.h
    ${CMAKE_CURRENT_LIST_DIR}/mesh/mesh3d.h
    ${CMAKE_CURRENT_LIST_DIR}/vptree.h
    ${CMAKE_CURRENT_LIST_DIR}/convexPolyhedron.h
    ${CMAKE_CURRENT_LIST_DIR}/conic.h
    ${CMAKE_CURRENT_LIST_DIR}/polygonPointIterator.h
#    ${CMAKE_CURRENT_LIST_DIR}/projection.h
    ${CMAKE_CURRENT_LIST_DIR}/gentryState.h
    ${CMAKE_CURRENT_LIST_DIR}/twoViewOptimalTriangulation.h
    ${CMAKE_CURRENT_LIST_DIR}/mesh/mesh3DDecorated.h
    ${CMAKE_CURRENT_LIST_DIR}/ellipse.h
    ${CMAKE_CURRENT_LIST_DIR}/ellipseFit.h
    ${CMAKE_CURRENT_LIST_DIR}/plane3dFit.h
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshClicker.h
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshFilter.h
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshCache.h
    ${CMAKE_CURRENT_LIST_DIR}/convexHull.h
    ${CMAKE_CURRENT_LIST_DIR}/convexHull3d.h
    ${CMAKE_CURRENT_LIST_DIR}/convexQuickHull.h
    ${CMAKE_CURRENT_LIST_DIR}/projectiveConvexQuickHull.h
    ${CMAKE_CURRENT_LIST_DIR}/pointCloud.h
    ${CMAKE_CURRENT_LIST_DIR}/halfspaceIntersector.h
    ${CMAKE_CURRENT_LIST_DIR}/orientedBox.h
    ${CMAKE_CURRENT_LIST_DIR}/plane.h
    ${CMAKE_CURRENT_LIST_DIR}/kdtree.h
    ${CMAKE_CURRENT_LIST_DIR}/beziercurve.h
    ${CMAKE_CURRENT_LIST_DIR}/raytrace/bspTree.h
    )

set(GEOMETRY_SOURCE_FILES
    ${CMAKE_CURRENT_LIST_DIR}/ellipticalApproximation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/axisAlignedBox.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rectangle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/triangulation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/polygons.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mesh/mesh3d.cpp
    ${CMAKE_CURRENT_LIST_DIR}/convexPolyhedron.cpp
    ${CMAKE_CURRENT_LIST_DIR}/conic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/polygonPointIterator.cpp
#    ${CMAKE_CURRENT_LIST_DIR}/projection.cpp
    ${CMAKE_CURRENT_LIST_DIR}/gentryState.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mesh/mesh3DDecorated.cpp
    ${CMAKE_CURRENT_LIST_DIR}/twoViewOptimalTriangulation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ellipse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ellipseFit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plane3dFit.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshClicker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshFilter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/mesh/meshCache.cpp
    ${CMAKE_CURRENT_LIST_DIR}/convexHull.cpp
    ${CMAKE_CURRENT_LIST_DIR}/convexHull3d.cpp
    ${CMAKE_CURRENT_LIST_DIR}/convexQuickHull.cpp
    ${CMAKE_CURRENT_LIST_DIR}/projectiveConvexQuickHull.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pointCloud.cpp
    ${CMAKE_CURRENT_LIST_DIR}/halfspaceIntersector.cpp
    ${CMAKE_CURRENT_LIST_DIR}/orientedBox.cpp
    ${CMAKE_CURRENT_LIST_DIR}/plane.cpp
    )

option(with_renderer "Should compile renderer" YES)

if(with_renderer)
    set(GEOMETRY_HEADER_FILES
        ${GEOMETRY_HEADER_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceableNodeWrapper.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceRenderer.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceObjects.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/perlinNoise.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/sdfRenderable.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/materialExamples.h
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/sdfRenderableObjects.h
        ${CMAKE_CURRENT_LIST_DIR}/renderer/attributedTriangleSpanIterator.h
        ${CMAKE_CURRENT_LIST_DIR}/renderer/simpleRenderer.h
        ${CMAKE_CURRENT_LIST_DIR}/renderer/geometryIterator.h
        )

    set(GEOMETRY_SOURCE_FILES
        ${GEOMETRY_SOURCE_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceRenderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceObjects.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/perlinNoise.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/sdfRenderable.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/materialExamples.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/raytraceableNodeWrapper.cpp
        ${CMAKE_CURRENT_LIST_DIR}/renderer/simpleRenderer.cpp
        ${CMAKE_CURRENT_LIST_DIR}/renderer/attributedTriangleSpanIterator.cpp
        ${CMAKE_CURRENT_LIST_DIR}/raytrace/sdfRenderableObjects.cpp
        )
endif()

set(GEOMETRY_HEADER_FILES ${GEOMETRY_HEADER_FILES} PARENT_SCOPE)
set(GEOMETRY_SOURCE_FILES ${GEOMETRY_SOURCE_FILES} PARENT_SCOPE)

