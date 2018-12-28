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

#include "core/geometry/polygons.h"
#include "core/geometry/line.h"
#include <iterator>

namespace corecvs {

//template<typename RealType, typename GeometryType, class SeparatorType>
//class BSPTreeNodeBase {


//};

class BSPTree2d {
    Line2d               partition;
    std::vector<Ray2d>   coincidentEdges;
    BSPTree2d            *front,
                         *back;
    BSPTree2d() {

    }

    void BSPDivide(BSPTree2d *tree, std::vector<Ray2d> edges)
    {
        std::vector<Ray2d>::iterator edgesIt = edges.begin();

        if (edgesIt == edges.end())
            return;
        Ray2d *root = &(*edgesIt);

        tree->partition = Line2d(*root);
        tree->coincidentEdges.push_back(*root);
        std::vector<Ray2d> frontEdges,
                           backEdges;

        ++edgesIt;
        while (edgesIt != edges.end()) {
            Ray2d *curEdge = &(*edgesIt);
            int result = classifyEdge(tree->partition, curEdge);

            switch (result) {
                case COINCIDENT:
                    tree->coincidentEdges.push_back(*curEdge);
                    break;
                case IN_BACK:
                    backEdges.push_back(*curEdge);
                    break;
                case IN_FRONT:
                    frontEdges.push_back(*curEdge);
                    break;
                case INTERCECT:
                    Ray2d *frontPart, *backPart;
                    SplitRay2d(curEdge, tree->partition, frontPart, backPart);
                    backEdges.push_back(*backPart);
                    frontEdges.push_back(*frontPart);
                    break;
            }
            ++edgesIt;
        }

        if (!frontEdges.empty()) {
            tree->front = new BSPTree2d;
            BSPDivide(tree->front, frontEdges);
        }

        if (!backEdges.empty()) {
            tree->back = new BSPTree2d;
            BSPDivide (tree->back, backEdges);
        }
    }
};


} // namespace corecvs

#endif // BSPRENDERER_H
