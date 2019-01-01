/**
 * \file BSPRenderer.h
 * \brief Renders scene using BSP-tree
 *
 * \ingroup ?
 * \date Dec, 2018
 * \author Mark Kovalev
 */

#ifndef BSPRENDERER_H
#define BSPRENDERER_H

#include <iterator>
#include <iostream>
#include <climits>
#include <cfloat>

#include "core/geometry/polygons.h"
#include "core/geometry/line.h"

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/polygonPointIterator.h"
#include "core/buffers/bufferFactory.h"

#define COINCIDENT  0
#define RIGHT       1
#define LEFT        2
#define INTERSECT   3
#define ERROR       -1

#if 1

namespace corecvs {

namespace BSPRenderer {

struct Linedef {
    unsigned long polygonIdx;
    int sidedef;
};

struct Sector {
    vector<Linedef> linedefs;
    Polygon space;
};

struct Level {
    vector<Sector> sectors;
};

class BSPNode2d {
public:
    Ray2d       *separator = nullptr;
    Polygon     leftBoundingBox,
                rightBoundingBox;
    BSPNode2d   *leftTree = nullptr,
                *rightTree = nullptr;
    Sector      *leftSector = nullptr,
                *rightSector = nullptr;

    BSPNode2d() {}

    BSPNode2d(Level& level)
    {
        BSPNodeBuilder(level);
    }

private:
    void BSPNodeBuilder(Level& level)
    {
        vector<Ray2d> allRays;
        Ray2d *bestSeparator;
        int bestCost = INT_MAX;
        Level leftLevel,
              rightLevel;

        for (Sector& curSector : level.sectors) {
            Polygon poly = curSector.space;
            for (unsigned long i = 0; i < poly.size(); ++i) {
                allRays.push_back(poly.getRay(i));
            }
        }

        for (Ray2d& curRay : allRays) {
            Level curLeftLevel,
                  curRightLevel;
            int cost,
                splits,
                sectorDiff,
                leftSectors,
                rightSectors;

            splits = SplitLevel(level, curRay,
                                curLeftLevel, curRightLevel);

            leftSectors  = int(curLeftLevel.sectors.size());
            rightSectors = int(curRightLevel.sectors.size());
            sectorDiff = abs(leftSectors - rightSectors);
            cost = 3 * splits + 2 * sectorDiff;

            if (cost < bestCost &&
                leftSectors != 0 &&
                rightSectors != 0)
            {
                bestSeparator = &curRay;
                leftLevel = curLeftLevel;
                rightLevel = curRightLevel;
            }
        }

        leftBoundingBox = GetBoundingBox(leftLevel);
        rightBoundingBox = GetBoundingBox(rightLevel);
        separator = new Ray2d(bestSeparator->a, bestSeparator->p);
        if (leftLevel.sectors.size() > 1) {
            leftTree = new BSPNode2d(leftLevel);
        } else {
            leftSector = new Sector;
            *leftSector = leftLevel.sectors[0];
        }

    }

    static int SplitLevel(Level& level, Ray2d& separator,
                          Level& leftPart, Level& rightPart)
    {
        Line2d sepLine(separator);
        int splits = 0;

        for (Sector& curSector : level.sectors) {
            Polygon poly = curSector.space;
            double t1, t2;

            separator.clip(poly, t1, t2);
            if (t2 > t1) {
                Sector leftSector,
                       rightSector;
                Vector2dd p1 = separator.getPoint(t1);
                Vector2dd p2 = separator.getPoint(t2);

                if (SplitSector(curSector, sepLine,
                                p1, p2,
                                leftSector, rightSector))
                {
                    ++splits;
                }

                if (leftSector.space.size() != 0)
                    leftPart.sectors.push_back(leftSector);

                if (rightSector.space.size() != 0)
                    rightPart.sectors.push_back(rightSector);
            } else {
                unsigned long i = 0;

                while (sepLine.side(poly.getPoint(i)) == 0)
                    ++i;

                if (sepLine.side(poly.getPoint(i)) == 1) {
                    rightPart.sectors.push_back(curSector);
                } else {
                    leftPart.sectors.push_back(curSector);
                }
            }
        }

        return splits;
    }

    static bool SplitSector(Sector& splitSec, Line2d& sepLine,
                            Vector2dd& enterPt, Vector2dd& exitPt,
                            Sector& leftPart, Sector& rightPart)
    {
        Polygon poly = splitSec.space;
        unsigned long enterIdx,
                      exitIdx;
        vector<int> origLineInfo(poly.size(), 0);
        unsigned long i;

        /* Check if tangent */
        for (i = 0; i < poly.size(); ++i) {
            Line2d polyLine = poly.getLine(i);

            if (polyLine.side(enterPt) == 0 &&
                polyLine.side(exitPt)  == 0)
            {
                if (polyLine.side(poly.getNextPoint(i + 1)) == 1)
                    rightPart = splitSec;
                else
                    leftPart = splitSec;

                return false;
            }

            if (polyLine.side(enterPt) == 0 &&
                enterPt != poly.getNextPoint(i))
            {
                 enterIdx = i;
            }

            if (polyLine.side(exitPt) == 0 &&
                enterPt != poly.getNextPoint(i))
            {
                exitIdx = i;
            }
        }

        for (Linedef& curLinedef : splitSec.linedefs) {
            origLineInfo[curLinedef.polygonIdx] = curLinedef.sidedef;
        }

        /* 1 -- to the right, -1 -- to the left */
        if (sepLine.side(poly.getNextPoint(enterIdx)) == 1) {
                rightPart = SectorFromPoly(poly, origLineInfo,
                                           enterIdx, exitIdx,
                                           enterPt, exitPt);
                leftPart = SectorFromPoly(poly, origLineInfo,
                                          exitIdx, enterIdx,
                                          exitPt, enterPt);
        } else {
                leftPart = SectorFromPoly(poly, origLineInfo,
                                          enterIdx, exitIdx,
                                          enterPt, exitPt);
                rightPart = SectorFromPoly(poly, origLineInfo,
                                           exitIdx, enterIdx,
                                           exitPt, enterPt);
        }

        return true;
    }

    static Sector SectorFromPoly(Polygon& poly, vector<int>& origLineInfo,
                                 unsigned long& startIdx, unsigned long& endIdx,
                                 Vector2dd& startPt, Vector2dd& endPt)
    {
        Sector outSector;
        Polygon outPoly;
        unsigned long i = startIdx;

        outPoly.push_back(startPt);
        while (i != endIdx) {
            outPoly.push_back(poly.getNextPoint(i));
            i = poly.getNextIndex(i);
        }
        outPoly.push_back(endPt);
        outSector.space = outPoly;

        for (i = 0; i < outPoly.size() - 1; ++i) {
            unsigned long j = (i + startIdx) % poly.size();
            if (origLineInfo[j] != 0) {
                Linedef curLinedef {j, origLineInfo[i]};
                outSector.linedefs.push_back(curLinedef);
            }
        }

        return outSector;
    }

    static Polygon GetBoundingBox(Level& level)
    {
        Polygon boundingBox;
        double xMin = DBL_MAX, xMax = 0.0,
               yMin = DBL_MAX, yMax = 0.0;

        for (Sector& curSector : level.sectors) {
            for (Vector2dd& curPoint : curSector.space) {
                if (curPoint.x() < xMin) xMin = curPoint.x();
                if (curPoint.x() > xMax) xMax = curPoint.x();
                if (curPoint.y() < yMin) yMin = curPoint.y();
                if (curPoint.y() > yMax) yMax = curPoint.y();
            }
        }

        boundingBox.push_back(Vector2dd(xMin, yMin));
        boundingBox.push_back(Vector2dd(xMin, yMax));
        boundingBox.push_back(Vector2dd(xMax, yMax));
        boundingBox.push_back(Vector2dd(xMax, yMin));

        return boundingBox;
    }

};

} // namespace BSPRenderer

} // namespace corecvs

#endif

#endif // BSPRENDERER_H
