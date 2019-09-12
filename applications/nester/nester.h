#ifndef NESTER_H
#define NESTER_H

#include <string>
#include <vector>
#include <list>
#include "core/fileformats/svgLoader.h"
#include "core/geometry/polygons.h"

/* Helpers  */
//declaration of same methods, working with list are in main_nester_test, cause for some reason header do not see list
void drawPolygons   (std::vector<corecvs::Polygon> inputPolygons, int h, int w, std::string bmpname);
void drawSvgPolygons(std::vector<corecvs::Polygon> inputPolygons, int h, int w, std::string svgName);
void addSubPolygons (corecvs::SvgShape *shape, std::vector<corecvs::Polygon> &inputPolygons);


/* new era*/
bool isInteriorROConvexPolBinSearch(const corecvs ::Vector2dd &Point, const corecvs :: Polygon &A);

bool isInteriorConvexPol(const corecvs ::Vector2dd &point, const corecvs :: Polygon &A);

bool hasBiggerLOArg(const corecvs ::Vector2dd &v1, const corecvs :: Vector2dd &v2);

corecvs::Polygon convexNFP(const corecvs::Polygon &A, const corecvs::Polygon &B);

corecvs::Rectangled innerFitPolygon(const corecvs::Polygon &A, const corecvs::Rectangled &R);

//same about bottomLeftPlacement, using list
/* helpful methods */


void doClockOrP(corecvs :: Polygon &A);

void showPolygon(const corecvs :: Polygon &A);

#endif // NESTER_H
