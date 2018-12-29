/**
 * \file main_test_draw.cpp
 * \brief This is the main file for the test draw
 *
 * \date Apr 19, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#include <iostream>
#include <random>
#include <fstream>
#include <cstdio>

#include "core/geometry/renderer/attributedTriangleSpanIterator.h"

#include "gtest/gtest.h"

#include "core/utils/global.h"

#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/fileformats/bmpLoader.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/geometry/renderer/simpleRenderer.h"
#include "core/geometry/mesh3d.h"
#include "core/cameracalibration/cameraModel.h"

#include "core/geometry/polygonPointIterator.h"
#include "core/buffers/bufferFactory.h"

#include "bspRenderer.h"

using namespace corecvs;

/*corecvs::Polygon getLevel1()
{
    for (int i = 0; i < 10; i++)
    {



    }

}*/


TEST(BSPRender, levelDraw)
{
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    corecvs::Polygon p;

    /* Create polygon of level */
    p.push_back(Vector2dd(59, 59));

    p.push_back(Vector2dd(299, 59));
    p.push_back(Vector2dd(239, 239));
    p.push_back(Vector2dd(479, 299));
    p.push_back(Vector2dd(479, 59));

    p.push_back(Vector2dd(659, 59));
    p.push_back(Vector2dd(659, 479));
    p.push_back(Vector2dd(539, 659));

    p.push_back(Vector2dd(479, 659));
    p.push_back(Vector2dd(479, 479));
    p.push_back(Vector2dd(299, 419));
    p.push_back(Vector2dd(299, 599));

    p.push_back(Vector2dd(59, 659));

    /* Snapshot of level */
    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawPolygon(p, RGBColor::White());
    BMPLoader().save("levelPolygon.bmp", buffer);

    /* Divide polygon into vector of rays and create BSP-tree */
    int i = 0;
    BSPRenderer::BSPTree2d tree;
    std::vector<Ray2d> edges = BSPRenderer::PolygonToRays(p);
    tree.BSPDivide(edges);
    /* Check our BSP-tree */
    BSPRenderer::DrawBSPTree(tree, p, i);

    delete_safe(buffer);
}
