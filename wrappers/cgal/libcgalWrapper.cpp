#include "libcgalWrapper.h"

#include <core/geometry/convexQuickHull.h>

Vector3dd toVector3dd(const Point_3 &p)
{
    return Vector3dd(p.x(), p.y(), p.z());
}

void LibCGALWrapper::drawPolyhedronToMesh(Polyhedron_3 &polyhedron, corecvs::Mesh3D &mesh)
{
    for (auto f = polyhedron.facets_begin(); f != polyhedron.facets_end(); ++f)
    {
        auto h = f->halfedge();
        Vector3dd p1 = toVector3dd(h->vertex()->point());
        Vector3dd p2 = toVector3dd(h->next()->vertex()->point());

        auto h1 = h->next()->next();
        while (h1 != h) {
            Vector3dd p = toVector3dd(h1->vertex()->point());
            mesh.addTriangle(p1, p2, p);
            h1 = h1->next();
        }
    }
}



ConvexQuickHull::HullFaces LibCGALWrapper::polyhedronToHullFaces(Polyhedron_3 &polyhedron)
{
    ConvexQuickHull::HullFaces toReturn;

    toReturn.reserve(polyhedron.size_of_facets());
    for (auto f = polyhedron.facets_begin(); f != polyhedron.facets_end(); ++f)
    {
        auto h = f->halfedge();
        Vector3dd p1 = toVector3dd(h->vertex()->point());
        Vector3dd p2 = toVector3dd(h->next()->vertex()->point());

        auto h1 = h->next()->next();
        while (h1 != h) {
            Vector3dd p = toVector3dd(h1->vertex()->point());
            h1 = h1->next();
            toReturn.push_back(ConvexQuickHull::HullFace(p1, p2, p));
        }
    }

    return toReturn;
}


ConvexQuickHull::HullFaces LibCGALWrapper::convexHull(const std::vector<Vector3dd> &points)
{
    vector<Point_3> pointCopy;

    pointCopy.reserve(points.size());
    for (size_t i = 0 ; i < points.size(); i++)
    {
        pointCopy.emplace_back(points[i].x(), points[i].y(), points[i].z());
    }

    Polyhedron_3 poly;
    // compute convex hull of non-collinear points
    CGAL::convex_hull_3(pointCopy.begin(), pointCopy.end(), poly);
    std::cout << "The first convex hull contains " << poly.size_of_vertices() << " vertices" << std::endl;

    return polyhedronToHullFaces(poly);
};
