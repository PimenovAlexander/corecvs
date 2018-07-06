#include "core/geometry/halfspaceIntersector.h"
#include "core/geometry/mesh3d.h"
#include "core/geometry/polygons.h"

namespace corecvs {

HalfspaceIntersector::HalfspaceIntersector()
{

}

#if 0
Polygon corecvs::HalfspaceIntersector::FromConvexPolygon(const ConvexPolygon &polygon)
{
  vector<Line2d> vertical;

  /* We don't need this. */
  //vector<Line2d> upper;
  //vector<Line2d> lower;

  vector<Vector2dd> upper;
  vector<Vector2dd> lower;

  for (const Line2d &line : polygon.faces)
  {
      if (line.isVertical()) vertical.push_back(line);
      if (line.y() > 0) upper.push_back(line.toDual());
      if (line.y() < 0) lower.push_back(line.toDual());
  }

  Polygon upperPol = ConvexHull::GrahamScan(upper);
  Polygon lowerPol = ConvexHull::GrahamScan(lower);

  double xmin = std::numeric_limits<double>::lowest();
  double xmax = std::numeric_limits<double>::max();


  for (size_t i = 0; i < vertical.size(); i++)
  {
      Line2d &l = vertical[i];


  }


  Mesh3D debug;
  debug.switchColor(true);
  debug.setColor(RGBColor::Red());
  for (size_t i = 0; i < upperPol.size(); i++)
  {
      Vector2dd p = upperPol[i];
      Line2d l = upperPol.getLine(i);

      debug.addPoint(Vector3dd(p, 0.0));
      debug.addPoint(Vector3dd(l.toDual() , 0.0));
  }

  debug.setColor(RGBColor::Blue());
  for (size_t i = 0; i < lowerPol.size(); i++)
  {
      Vector2dd p = lowerPol[i];
      Line2d l = lowerPol.getLine(i);

      debug.addPoint(Vector3dd(p, 0.0));
      debug.addPoint(Vector3dd(l.toDual() , 0.0));

  }
  debug.dumpPLY("halfspace.ply");

  return upperPol;
}
#endif

ConvexPolygon HalfspaceIntersector::FromConvexPolygonCP(const ConvexPolygon &polygon)
{
    ConvexPolygon  toReturn;
    vector<Vector3dd> dual;

    dual.reserve(polygon.faces.size());
    for (const Line2d &line : polygon.faces)
    {
        dual.push_back(line.toDualP());
    }

    ProjectivePolygon pol = ConvexHull::GrahamScan(dual);
    toReturn.faces.reserve(pol.size());
    for (const Vector3dd &p: pol)
    {
        toReturn.faces.push_back(p);
    }
    return toReturn;
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

} // namespace corecvs
