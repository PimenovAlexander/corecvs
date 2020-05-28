/**
 * \file main_test_convexduality.cpp
 * \brief This is the main file for the test convexduality 
 *
 * \date мая 26, 2019
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <core/geometry/convexHull.h>
#include <core/geometry/mesh/mesh3d.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"


using namespace std;
using namespace corecvs;

Polygon FromConvexPolygon(const ConvexPolygon &polygon,  Mesh3D &debug)
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

  debug.switchColor(true);
  for (size_t i = 0; i < upperPol.size(); i++)
  {
      Vector2dd p = upperPol[i];
      Line2d l = upperPol.getLine(i);

      debug.setColor(RGBColor::Red());
      debug.addPoint(Vector3dd(p, 0.0));

      debug.setColor(RGBColor::Pink());
      debug.addPoint(Vector3dd(l.toDual() , 0.0));
  }

  for (size_t i = 0; i < lowerPol.size(); i++)
  {
      Vector2dd p = lowerPol[i];
      Line2d l = lowerPol.getLine(i);

      debug.setColor(RGBColor::Blue());
      debug.addPoint(Vector3dd(p, 0.0));

      debug.setColor(RGBColor::Cyan());
      debug.addPoint(Vector3dd(l.toDual() , 0.0));

  }

  return upperPol;
}

TEST(convexduality, testconvexduality)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    mesh.setColor(RGBColor::Red());

    Polygon p1 = { { 99,  60}, { 10, 100}, {  10,  10}};
    Polygon p2 = { { 11,  60}, {100,  10}, { 100, 100}};

    int rotation = 29;
    Vector2dd shift(100, -100);
    int permutation = 1469;

    Matrix33 transform = Matrix33::ScaleProj(1 / 20.0) * Matrix33::ShiftProj(shift) * Matrix33::RotationZ(degToRad(rotation));
    p1.transform(transform);
    p2.transform(transform);

    cout << "Rotation: " << rotation << endl;
    cout << "Shift: " << shift << endl;
    cout << "Permutation: " << permutation << endl;
    cout << "Matrix:\n" << transform << endl;

    mesh.setColor(RGBColor::Red());
    for (size_t i = 0; i < p1.size(); i++) {
        mesh.addLine(Vector3dd(p1.getPoint(i), 0.9), Vector3dd(p1.getNextPoint(i), 0.9));
    }
    mesh.setColor(RGBColor::Blue());
    for (size_t i = 0; i < p2.size(); i++) {
        mesh.addLine(Vector3dd(p2.getPoint(i), 0.9), Vector3dd(p2.getNextPoint(i), 0.9));
    }

    Rectangled holder = Rectangled::Empty();
    holder.extendToFit(p1);
    holder.extendToFit(p2);

    ConvexPolygon cp1 = p1.toConvexPolygon();
    ConvexPolygon cp2 = p2.toConvexPolygon();

    ConvexPolygon cp3 = ConvexPolygon::merge(cp1, cp2);

    cout << "First" << cp1 << endl;
    cout << "Second" << cp2 << endl;

    cout << "Initail" << cp3 << endl;
    ConvexPolygon pcp3 = cp3.permutate(permutation);
    cout << "Permutated" << pcp3 << endl;

    FromConvexPolygon(pcp3, mesh);
    mesh.dumpPLY("halfspace.ply");
}
