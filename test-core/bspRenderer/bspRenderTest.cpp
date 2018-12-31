/**
 * \file main_test_draw.cpp
 * \brief This is the main file for the test draw
 *
 * \date Apr 19, 2011
 * \author alexander
 *
 * \ingroup autotest
 */

#include "bspRenderTest.h"

using namespace corecvs;

TEST(BSPRender, levelDraw)
{
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::White());
    Polygon p;

    p.push_back(Vector2dd(59, 59));
    p.push_back(Vector2dd(59, 659));

//    p.push_back(Vector2dd(299, 599));
//    p.push_back(Vector2dd(299, 419));
//    p.push_back(Vector2dd(479, 479));
//    p.push_back(Vector2dd(479, 659));

    p.push_back(Vector2dd(539, 659));
    p.push_back(Vector2dd(659, 479));
    p.push_back(Vector2dd(659, 59));

//    p.push_back(Vector2dd(479, 59));
//    p.push_back(Vector2dd(479, 299));
//    p.push_back(Vector2dd(239, 239));
//    p.push_back(Vector2dd(299, 59));

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawPolygon(p, RGBColor::Black());

    for (int i = 0; i < p.size(); i++)
    {
        Vector2dd p1 = p.getPoint(i);
        Vector2dd p2 = p.getNextPoint(i);

        Vector2dd center = (p1 + p2) / 2.0;
        Vector2dd decal = p.getNormal(i).normalised();
        Vector2dd number = center - decal * 8;
        painter.drawFormat((unsigned short) round(number.x()),
                           (unsigned short) round(number.y()),
                           RGBColor::Purple(), 2, "%i", i);
        decal = center + decal * 8;
        buffer->drawLine(center, decal, RGBColor::Black());
    }

    for (int i = 1; i < 10; i++)
    {
        Ray2d ray( Vector2dd::FromPolar( M_PI / 20.0 * i + 0.02, 100), Vector2dd(140,180));

        Vector2dd p1 = ray.getPoint(0.0);
        Vector2dd p2 = ray.getPoint(8.0);
        buffer->drawLine(p1, p2, RGBColor::Yellow());

        for (int j = 0; j < 8; j++)
        {
            Vector2dd p = ray.getPoint(j);
            buffer->drawCrosshare1(p.x(), p.y(), RGBColor::Cyan());
        }

        double t1, t2;
        ray.clip<Polygon>(p, t1, t2);

        p1 = ray.getPoint(t1);
        p2 = ray.getPoint(t2);

        cout << "t1 = " << t1 << endl;
        cout << "t2 = " << t2 << endl;
        cout << "p1 = " << p1 << endl;
        cout << "p2 = " << p2 << endl;

        buffer->drawLine(p1, p2,
                       t1 > t2 ? RGBColor::Red() : RGBColor::Green());

    }

    BMPLoader().save("levelPolygon.bmp", buffer);

#if 0

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

#endif

    delete_safe(buffer);
}
