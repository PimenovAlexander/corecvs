#ifndef LIBCGALWRAPPER_H
#define LIBCGALWRAPPER_H

#include <CGAL/convex_hull_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Convex_hull_3/dual/halfspace_intersection_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Surface_mesh.h>

#include <core/geometry/convexQuickHull.h>
#include <core/geometry/mesh/mesh3d.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel  K;
typedef CGAL::Polyhedron_3<K>                     Polyhedron_3;
typedef K::Point_3 Point_3;

class LibCGALWrapper
{
public:


    static void                       drawPolyhedronToMesh (Polyhedron_3 &polyhedron, corecvs::Mesh3D &mesh);
    static ConvexQuickHull::HullFaces polyhedronToHullFaces(Polyhedron_3 &polyhedron);
    static ConvexQuickHull::HullFaces convexHull(const std::vector<Vector3dd> &points);
};

#endif // LIBCGALWRAPPER_H
