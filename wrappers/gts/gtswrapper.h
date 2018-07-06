#ifndef GTSWRAPPER_H
#define GTSWRAPPER_H

#include <vector>
#include <list>
#include <stdlib.h>

#include "core/geometry/triangulation.h"

using Triangulation::Edge;
using Triangulation::Point;

std::vector<Edge>    triangulateGts(const std::vector<Point> &points);

#endif // GTSWRAPPER_H
