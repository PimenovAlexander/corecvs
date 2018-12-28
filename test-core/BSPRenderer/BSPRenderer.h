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

#include "core/geometry/polygons.h"
#include "core/geometry/line.h"

#define COINCIDENT  0
#define RIGHT       1
#define LEFT        2
#define INTERSECT   3
#define ERROR       -1

namespace corecvs {

#if 1

class BSPTree2d {
public:
    Ray2d                *separatorEdge = nullptr;
    std::vector<Ray2d>   coincidentEdges;
    BSPTree2d            *front = nullptr,
                         *back = nullptr;

    BSPTree2d() {}

    void BSPDivide(std::vector<Ray2d> edges)
    {
        std::vector<Ray2d>::iterator edgesIt = edges.begin();

        if (edgesIt == edges.end()) {
            cout << "No more edges left!\n";
            return;
        }

        separatorEdge = &(*edgesIt);
        coincidentEdges.push_back(*separatorEdge);
        std::vector<Ray2d> leftEdges,
                           rightEdges;

        ++edgesIt;
        while (edgesIt != edges.end()) {
            Ray2d *curEdge = &(*edgesIt);
            int result = classifyEdge(separatorEdge, curEdge);

            switch (result) {
                case COINCIDENT:
                    coincidentEdges.push_back(*curEdge);
                    break;
                case RIGHT:
                    rightEdges.push_back(*curEdge);
                    break;
                case LEFT:
                    leftEdges.push_back(*curEdge);
                    break;
                case INTERSECT:
                {
                    Ray2d *rightPart = nullptr,
                          *leftPart  = nullptr;
                    SplitRay2d(curEdge, separatorEdge, rightPart, leftPart);
                    rightEdges.push_back(*leftPart);
                    leftEdges.push_back(*rightPart);
                    break;
                }
                case ERROR:
                    cout << "\n\nSOMETHING WENT TERRIBLY WRONG HERE!\n\n";
                    break;
            }
            ++edgesIt;
        }

        if (!leftEdges.empty()) {
            front = new BSPTree2d();
            front->BSPDivide(leftEdges);
        }

        if (!rightEdges.empty()) {
            back = new BSPTree2d();
            back->BSPDivide(rightEdges);
        }
    }

    static int classifyEdge(Ray2d *separatorEdge, Ray2d *curEdge)
    {
        Line2d separatorLine = Line2d(*separatorEdge);
        int startPt = separatorLine.side(curEdge->getStart());
        int endPt = separatorLine.side(curEdge->getEnd());

        if (startPt == endPt) {
            switch (startPt) {
                case 1:
                    return RIGHT;
                case 0:
                    return COINCIDENT;
                case -1:
                    return LEFT;
            }
        } else if (startPt == 0) {
            if (endPt == 1)
                return RIGHT;
            else
                return LEFT;
        } else if (endPt == 0) {
            if (startPt == 1)
                return RIGHT;
            else
                return LEFT;
        } else
            return INTERSECT;

        return ERROR;
    }

    static void SplitRay2d(Ray2d *curEdge, Ray2d *separatorEdge,
                           Ray2d *rightPart, Ray2d *leftPart)
    {
        Line2d separatorLine = Line2d(*separatorEdge);
        Line2d curLine = Line2d(*curEdge);
        int startPt = separatorLine.side(curEdge->getStart());
        Vector2dd interPt = separatorLine.intersectWith(curLine);

        if (startPt == 1) {
            *rightPart = Ray2d::FromPoints(curEdge->getStart(), interPt);
            *leftPart  = Ray2d::FromPoints(interPt, curEdge->getEnd());
        } else {
            *leftPart  = Ray2d::FromPoints(curEdge->getStart(), interPt);
            *rightPart = Ray2d::FromPoints(interPt, curEdge->getEnd());
        }
    }
};

#endif

} // namespace corecvs

#endif // BSPRENDERER_H
