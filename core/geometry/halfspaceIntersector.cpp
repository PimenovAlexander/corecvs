#include "core/geometry/halfspaceIntersector.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/polygons.h"
#include "core/geometry/convexHull.h"

namespace corecvs {

HalfspaceIntersector::HalfspaceIntersector()
{

}


ConvexPolygon HalfspaceIntersector::Simplify(const ConvexPolygon &polygon)
{
    ProjectivePolygon pp = ProjectivePolygon::FromConvexPolygon(polygon);
    ProjectivePolygon pps;
    if (!ConvexHull::GiftWrap(pp, pps))
        return ConvexPolygon();
    return pps.toConvexPolygon();
}



ProjectivePolygon HalfspaceIntersector::FromConvexPolygonP(const ConvexPolygon &polygon)
{
  vector<Vector3dd> dual;

  dual.reserve(polygon.faces.size());
  for (const Line2d &line : polygon.faces)
  {
      dual.push_back(line.toDualP());
  }

  ProjectivePolygon pol = ConvexHull::GrahamScan(dual);
  /* Pol now have projective represenation of lines. We need to go to point domain */

  ProjectivePolygon result;
  result.reserve(pol.size());
  for (size_t i = 0; i < pol.size(); i++)
  {
      Vector3dd &p1 = pol[i];
      Vector3dd &p2 = pol[(i + 1) % pol.size()];

      result.push_back(p1 ^ p2); /* Two lines */
  }


#if 0
  Mesh3D debug;
  debug.switchColor(true);
  debug.setColor(RGBColor::Red());
  for (size_t i = 0; i < dual.size(); i++)
  {
      debug.addPoint(dual[i]);
  }

  for (size_t i = 0; i < pol.size(); i++)
  {
      debug.setColor(RGBColor::Blue());
      debug.addPoint(Vector3dd(pol[i].normalisedProjective().xy(), 0.0));


      debug.setColor(RGBColor::Green());
      debug.addPoint(Vector3dd(result[i].normalisedProjective().xy(), 0.0));
  }

  debug.addOrts(50.0);
  debug.dumpPLY("halfspace-p.ply");
#endif

  return result;

}


ConvexQuickHull::HullFaces HalfspaceIntersector::FromConvexPolyhedron(const ConvexPolyhedron &polygon)
{
    vector<Plane3d> vertical;
    /* We don't need this. */
    //vector<Line2d> upper;
    //vector<Line2d> lower;

    vector<Vector3dd> upper;
    vector<Vector3dd> lower;

    for (const Plane3d &plane : polygon.faces)
    {
        if (plane.c() == 0.0) vertical.push_back(plane);
        if (plane.c() > 0) upper.push_back(plane.toDual());
        if (plane.c() < 0) lower.push_back(plane.toDual());
    }

    ConvexQuickHull::HullFaces upperPol = ConvexQuickHull::quickHull(upper);
    ConvexQuickHull::HullFaces lowerPol = ConvexQuickHull::quickHull(lower);


    for (size_t i = 0; i < vertical.size(); i++)
    {
        Plane3d p = vertical[i];

    }


    Mesh3D debug;
    debug.switchColor(true);
    /* Trace inputs */

    debug.setColor(RGBColor::Yellow());
    for (size_t i = 0; i < upper.size(); i++)
    {
        debug.addPoint(upper[i]);
    }


    debug.setColor(RGBColor::Green());
    for (size_t i = 0; i < lower.size(); i++)
    {
        debug.addPoint(lower[i]);
    }



    /* Trace remaining */
    debug.setColor(RGBColor::Red());
    for (size_t i = 0; i < upperPol.size(); i++)
    {
        ConvexQuickHull::HullFace f = upperPol[i];
        debug.addPoint(f.getPlane().toDual());
    }

    debug.setColor(RGBColor::Blue());
    for (size_t i = 0; i < lowerPol.size(); i++)
    {
        ConvexQuickHull::HullFace f = lowerPol[i];
        debug.addPoint(f.getPlane().toDual());

    }
    debug.dumpPLY("halfspace3d.ply");


    ConvexQuickHull::HullFaces dummy;
    return dummy;

}

ConvexPolyhedron HalfspaceIntersector::FromConvexPolyhedronCP(const ConvexPolyhedron &polyhedron)
{
    vector<ProjectiveCoord4d> dual;

    dual.reserve(polyhedron.faces.size());
    for (const Plane3d &plane : polyhedron.faces)
    {
        dual.push_back(ProjectiveCoord4d(plane.a(), plane.b(), plane.c(), plane.d()));
    }

    cout<< "start ProjectiveConvexHull"<<endl;
    ProjectiveConvexQuickHull::HullFaces facesD = ProjectiveConvexQuickHull::quickHull(dual);
#if 1 //return verticies and faces
    //get points from conex hull
    vector<Vector3dd> verticies;
    cout<<"get vertices"<<endl;
    for(ProjectiveConvexQuickHull::HullFace &face: facesD)  
    {
        Matrix33  A = Matrix33::FromRows(
                    face.plane.p1().xyz(),
                    face.plane.p2().xyz(),
                    face.plane.p3().xyz());

        Vector3dd b(face.plane.p1().w(), face.plane.p2().w(), face.plane.p3().w());

        if(A.det() != 0)
        {
            verticies.push_back(-A.inv() * b);
            cout<< "Add point: "<< verticies.back() <<endl;
        }
    }
    return ConvexPolyhedron(verticies);

#else //return only faces
    
    vector<ProjectiveCoord4d> pointsD;
    for(ProjectiveConvexQuickHull::HullFace &face: facesD)  
    {
        pointsD.push_back(face.plane.p1());
        pointsD.push_back(face.plane.p2());
        pointsD.push_back(face.plane.p3());
    }
    std::sort(pointsD.begin(), pointsD.end(), [](ProjectiveCoord4d &a, ProjectiveCoord4d &b)
        {
            if (a.x() < b.x()) return true;
            if (a.x() > b.x()) return false;

            if (a.y() < b.y()) return true;
            if (a.y() > b.y()) return false;

            if (a.z() < b.z()) return true;
            if (a.z() > b.z()) return false;

            if (a.w() < b.w()) return true;
            return false;
        }
    );
    pointsD.erase(std::unique(pointsD.begin(), pointsD.end()), pointsD.end());

    ConvexPolyhedron result;
    result.faces.reserve(pointsD.size());
    cout<< "Intersection result:"<<endl;
    for(ProjectiveCoord4d &point:pointsD)
    {
        result.faces.push_back(Plane3d(point.x(), point.y(), point.z(), point.w()));
        cout<<Plane3d(point.x(), point.y(), point.z(), point.w())<<endl;
    }
    return result;
#endif
}


} // namespace corecvs
