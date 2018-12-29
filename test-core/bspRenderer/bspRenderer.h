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

namespace corecvs {

#if 1

namespace BSPRenderer {

class BSPTree2d {
public:
    Ray2d               *separatorEdge = nullptr;
    std::vector<Ray2d>  coincidentEdges;
    BSPTree2d           *rightTree = nullptr,
                        *leftTree = nullptr;

    std::vector<Ray2d>  rightEdges,
                        leftEdges;

    BSPTree2d() {}

    void BSPDivide(std::vector<Ray2d> &edges)
    {
        std::vector<Ray2d>::iterator edgesIt = edges.begin();

        if (edgesIt == edges.end()) {
            cout << "No more edges left!\n";
            return;
        }

        separatorEdge = &(*edgesIt);
        coincidentEdges.push_back(*separatorEdge);

        ++edgesIt;
        while (edgesIt != edges.end()) {
            Ray2d *curEdge = &(*edgesIt);
            int result = ClassifyEdge(separatorEdge, curEdge);

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
                    Ray2d *rightPart = (Ray2d *) malloc(sizeof(Ray2d)),
                          *leftPart  = (Ray2d *) malloc(sizeof(Ray2d));
                    SplitRay2d(curEdge, separatorEdge, rightPart, leftPart);
                    rightEdges.push_back(*rightPart);
                    leftEdges.push_back(*leftPart);
                    break;
                }
                case ERROR:
                    cout << "\n\nSOMETHING WENT TERRIBLY WRONG HERE!\n\n";
                    break;
            }
            ++edgesIt;
        }

        if (!rightEdges.empty()) {
            rightTree = new BSPTree2d();
            rightTree->BSPDivide(rightEdges);
        }

        if (!leftEdges.empty()) {
            leftTree = new BSPTree2d();
            leftTree->BSPDivide(leftEdges);
        }
    }

    static int ClassifyEdge(Ray2d *separatorEdge, Ray2d *curEdge)
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

static void DrawBSPTree(BSPTree2d &tree, Polygon poly, int& i) {
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    std::string out("BSPSnap" + std::to_string(i) + ".bmp");
    AbstractPainter<RGB24Buffer> painter(buffer);

    painter.drawPolygon(poly, RGBColor::White());

    buffer->drawLine(tree.separatorEdge->getStart(),
                     tree.separatorEdge->getEnd(),
                     RGBColor::Red());
    for (auto& it : tree.coincidentEdges) {
        if (it.getStart() != tree.separatorEdge->getStart()
            && it.getEnd() != tree.separatorEdge->getEnd())
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
        BSPRenderer::DrawBSPTree(*(tree.rightTree), poly,  ++i);
    if(tree.leftTree != nullptr)
        BSPRenderer::DrawBSPTree(*(tree.leftTree), poly, ++i);
}

} // namespace BSPRenderer

#endif

} // namespace corecvs

#endif // BSPRENDERER_H
