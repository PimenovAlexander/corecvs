#ifndef GTSWRAPPER_H
#define GTSWRAPPER_H

#include <vector>
#include <list>
#include <triangulation.h>
#include <stdlib.h>


using Triangulation::Edge;
using Triangulation::Point;

std::vector<Edge>    triangulateGts(const vector<Point> &points);

#endif // GTSWRAPPER_H
