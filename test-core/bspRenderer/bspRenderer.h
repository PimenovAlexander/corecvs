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
    Segment2d line;
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
    /* Every node of tree contains:
     * Edge to separate space
     * Edges coincident with separator (at least separator itself)
     * Pointers to right and left subtrees
     * Vectors of right and left edges relative to the separator
     **/
    Ray2d       *separator = nullptr;
    Polygon     leftBoundingBox,
                rightBoundingBox;
    BSPNode2d   *rightTree = nullptr,
                *leftTree = nullptr;
    Sector      *left = nullptr,
                *right = nullptr;

    BSPNode2d() {}

    /* Main method of constructing BSP-tree
     * Needs vector of rays as input */
    void BSPNodeBuilder(Level& level)
    {
        vector<Polygon> allPolygons;
        vector<Ray2d> allRays;
        Ray2d *bestSeparator;
        int bestCost = INT_MAX;
        Level leftPart,
              rightPart;

        for (Sector& curSector : level.sectors) {
            allPolygons.push_back(curSector.space);
            for (Linedef& curLinedef : curSector.linedefs) {
                allRays.push_back(Ray2d(curLinedef.line));
            }
        }

        for (Ray2d& curRay : allRays) {
            Level curLeftPart,
                  curRightPart;
            int cost,
                clipsAmount,
                sectorDiff;

            clipsAmount = LevelPartition(level, curRay,
                                         curLeftPart, curRightPart);


        }

    }

    static int LevelPartition(Level& level, Ray2d& separator,
                              Level& leftPart, Level& rightPart)
    {
        int clips = 0;

        for (Sector& curSector : level.sectors) {
            Polygon poly = curSector.space;
            double t1, t2;

            separator.clip(poly, t1, t2);
            if (t2 > t1) {
                Vector2dd p1 = separator.getPoint(t1);
                Vector2dd p2 = separator.getPoint(t2);
                int enterIdx = -1,
                    exitIdx  = -2;

                /* Check if tangent */
                for (int i = 0; i < poly.size(); ++i) {
                    Line2d polyLine = poly.getLine(i);

                    if (polyLine.side(p1) == 0) enterIdx = i;
                    if (polyLine.side(p2) == 0) exitIdx = i;
                    if (enterIdx == exitIdx) continue;
                }


                ++clips;
            }
        }

        return clips;
    }

    static int ClassifyEdge(Ray2d *separatorEdge, Ray2d *curEdge)
    {
        Line2d separatorLine = Line2d(*separatorEdge);
        /* Positions of start and end of ray relative to the separatorLine */
        int startPt = separatorLine.side(curEdge->getStart());
        int endPt = separatorLine.side(curEdge->getEnd());

        /* If both points on the same side => ray on that side */
        if (startPt == endPt) {
            switch (startPt) {
                case 1:
                    return RIGHT;
                case 0:
                    return COINCIDENT;
                case -1:
                    return LEFT;
            }
        }
        /* If some point on separator and other's not => consider ray left or right */
        else if (startPt == 0) {
            if (endPt == 1)
                return RIGHT;
            else
                return LEFT;
        } else if (endPt == 0) {
            if (startPt == 1)
                return RIGHT;
            else
                return LEFT;
        }
        /* If points on different sides of separator => ray intersects separator */
        else
            return INTERSECT;

        /* In case something goes wrong */
        return ERROR;
    }

    static void SplitRay2d(Ray2d *curEdge, Ray2d *separatorEdge,
                           Ray2d *rightPart, Ray2d *leftPart)
    {
        Line2d separatorLine = Line2d(*separatorEdge);
        Line2d curLine = Line2d(*curEdge);
        int startPt = separatorLine.side(curEdge->getStart());
        /* Point of intertsection of lines, based on separatorEdge and curEdge */
        Vector2dd interPt = separatorLine.intersectWith(curLine);

        /* Split depends on which side startPt is,
         * because we need to save direction of splitted rays */
        if (startPt == 1) {
            *rightPart = Ray2d::FromPoints(curEdge->getStart(), interPt);
            *leftPart  = Ray2d::FromPoints(interPt, curEdge->getEnd());
        } else {
            *leftPart  = Ray2d::FromPoints(curEdge->getStart(), interPt);
            *rightPart = Ray2d::FromPoints(interPt, curEdge->getEnd());
        }
    }
};

/* Just iteration on edges of polygon to convert into vector of rays */
static std::vector<Ray2d> PolygonToRays(Polygon &poly)
{
    int i, pSize = poly.size();
    std::vector<Ray2d> rays;

    for (i = 0; i < pSize; ++i) {
        Vector2dd start = poly.getPoint(i);
        Vector2dd end = poly.getNextPoint(i);
        rays.push_back(Ray2d::FromPoints(start, end));
    }

    return rays;
}

/* This method returns picture of current node of BSP-tree:
 * Red - separator edge
 * Blue - edges coincident with separator
 * Green - edges to the right of separator
 * Yellow - edges to the left of separator
 * White - edges not processed in current node (they're already somwhere in tree)*/
static void DrawBSPTree(BSPNode2d &tree, Polygon poly, int& i) {
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    std::string out("BSPSnap" + std::to_string(i) + ".bmp");
    AbstractPainter<RGB24Buffer> painter(buffer);

    painter.drawPolygon(poly, RGBColor::White());

    buffer->drawLine(tree.separator->getStart(),
                     tree.separator->getEnd(),
                     RGBColor::Red());
    for (auto& it : tree.coincidentEdges) {
        if (it.getStart() != tree.separator->getStart()
            && it.getEnd() != tree.separator->getEnd())
        buffer->drawLine(it.getStart(),
                         it.getEnd(),
                         RGBColor::Blue());
    }
    for (auto& it : tree.rightEdges) {
        buffer->drawLine(it.getStart(),
                         it.getEnd(),
                         RGBColor::Green());
    }
    for (auto& it : tree.leftEdges) {
        buffer->drawLine(it.getStart(),
                         it.getEnd(),
                         RGBColor::Yellow());
    }
    BMPLoader().save(out, buffer);

    if(tree.rightTree != nullptr)
        BSPRenderer::DrawBSPTree(*(tree.rightTree), poly, ++i);
    if(tree.leftTree != nullptr)
        BSPRenderer::DrawBSPTree(*(tree.leftTree), poly, ++i);
}

} // namespace BSPRenderer

} // namespace corecvs

#endif

#endif // BSPRENDERER_H
