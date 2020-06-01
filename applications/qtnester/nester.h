#ifndef NESTER_H
#define NESTER_H
#include <string>
#include <vector>
#include <list>
#include "core/fileformats/svgLoader.h"
#include "core/geometry/polygons.h"
/* Helpers  */
void drawPolygons                  (std::vector<corecvs::Polygon> inputPolygons,
                                    int h, int w, std::string bmpname);

void drawSvgPolygons               (std::vector<corecvs::Polygon> inputPolygons,
                                    int h, int w, std::string svgName);

void drawPolygons                  (std::list <corecvs::Polygon> inputPolygons,
                                    int h, int w, std::string bmpname);

void drawSvgPolygons               (std::list <corecvs::Polygon> inputPolygons,
                                    int h, int w, std :: string svgName);

void addSubPolygons                (corecvs::SvgShape *shape,
                                    std::vector<corecvs::Polygon> &inputPolygons);
/* new era*/
bool isInteriorROConvexPolBinSearch(const corecvs::Vector2dd &Point,
                                    const corecvs::Polygon &A);

bool isInteriorConvexPol           (const corecvs::Vector2dd &point,
                                    const corecvs::Polygon &A);

bool hasBiggerLOArg                (const corecvs::Vector2dd &v1,
                                    const corecvs::Vector2dd &v2);

corecvs::Polygon convexNFP         (const corecvs::Polygon &A,  // ONE WILL BE WITH STEPS ANOTHER WILL NOT
                                    const corecvs::Polygon &B);

corecvs::Polygon convexNFPSaturated (const corecvs::Polygon &A,
                                     const corecvs::Polygon &B,
                                     size_t steps);

corecvs::Rectangled innerFitPolygon(const corecvs::Polygon &A,
                                    const corecvs::Rectangled &R);

void bottomLeftPlacement           (std :: list <corecvs::Polygon> &inp,
                                    corecvs::Rectangled &bin);

int getTopRightIndex               (const corecvs::Polygon &A);
/* helpful methods */
void doClockOrP                    (corecvs::Polygon &A);

void showPolygon                   (const corecvs::Polygon &A);

corecvs::Polygon getHomotheticPolygon      (corecvs::Polygon& p, double epsil);

void vinilPlacementNester          (std::list<corecvs::Polygon> &inputList,
                                    corecvs::Rectangled &bin, double epsil,
                                    bool lowTheMasses, int whichPlacement,
                                    double costParameter, size_t rotations);

void bruteBL                       (std::list <corecvs :: Polygon> &inp,
                                    corecvs :: Rectangled &bin,
                                    size_t rotatesAmount);

double heightOfPolygon             (const corecvs::Polygon& pol);

corecvs::Polygon polFromRec        (const corecvs::Rectangled &R);


void bruteHeightBL                 (std::list <corecvs :: Polygon> &inp,
                                    corecvs :: Rectangled &bin, size_t rotatesAmount,
                                    double heightCost);

double getMaxValueY                (const std::list<corecvs::Polygon> &inputList);

std::list<corecvs::Polygon>loadPolygonListDXF (const std::string &name);

std::list<corecvs::Polygon> loadPolygonListSVG(const std::string &name);

#endif // NESTER_H
