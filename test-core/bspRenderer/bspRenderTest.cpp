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

TEST(BSPRender, BSPTest)
{
    int h = 720;
    int w = 720;
    RGB24Buffer buffer(h, w, RGBColor::Black());
    Polygon p1, p2, p3, p4, p5, p6, p7, p8;
    BSPRenderer::BSPNode2d tree;
    BSPRenderer::Level level;
    BSPRenderer::Sector s1, s2, s3, s4, s5, s6, s7, s8;
    vector<BSPRenderer::Linedef> l1, l2, l3, l4, l5, l6, l7, l8;

    p1.push_back(Vector2dd(60 , 60 ));
    p1.push_back(Vector2dd(60 , 240));
    p1.push_back(Vector2dd(240, 120));
    p1.push_back(Vector2dd(240, 60 ));
    l1 = {{1}, {2}, {1}, {1}};
    s1.space = p1;
    s1.linedefs = l1;   

    p2.push_back(Vector2dd(240, 120));
    p2.push_back(Vector2dd(60 , 240));
    p2.push_back(Vector2dd(60 , 540));
    p2.push_back(Vector2dd(150, 510));
    p2.push_back(Vector2dd(240, 480));
    l2 = {{2}, {1}, {2}, {1}, {1}};
    s2.space = p2;
    s2.linedefs = l2;

    p3.push_back(Vector2dd(300, 240));
    p3.push_back(Vector2dd(240, 120));
    p3.push_back(Vector2dd(240, 360));
    l3 = {{1}, {0}, {0}};
    s3.space = p3;
    s3.linedefs = l3;

    p4.push_back(Vector2dd(420, 240));
    p4.push_back(Vector2dd(300, 240));
    p4.push_back(Vector2dd(240, 360));
    p4.push_back(Vector2dd(420, 360));
    l4 = {{1}, {0}, {1}, {2}};
    s4.space = p4;
    s4.linedefs = l4;

    p5.push_back(Vector2dd(420, 120));
    p5.push_back(Vector2dd(420, 240));
    p5.push_back(Vector2dd(420, 360));
    p5.push_back(Vector2dd(420, 480));
    p5.push_back(Vector2dd(420, 660));
    p5.push_back(Vector2dd(540, 660));
    p5.push_back(Vector2dd(660, 540));
    p5.push_back(Vector2dd(600, 60 ));
    l5 = {{1}, {2}, {1}, {0}, {1}, {1}, {1}, {1}};
    s5.space = p5;
    s5.linedefs = l5;

    p6.push_back(Vector2dd(420, 480));
    p6.push_back(Vector2dd(240, 540));
    p6.push_back(Vector2dd(240, 660));
    p6.push_back(Vector2dd(420, 660));
    l6 = {{1}, {1}, {1}, {0}};
    s6.space = p6;
    s6.linedefs = l6;

    p7.push_back(Vector2dd(150, 510));
    p7.push_back(Vector2dd(60 , 540));
    p7.push_back(Vector2dd(60 , 660));
    p7.push_back(Vector2dd(150, 660));
    l7 = {{2}, {1}, {1}, {1}};
    s7.space = p7;
    s7.linedefs = l7;

    p8.push_back(Vector2dd(660, 60 ));
    p8.push_back(Vector2dd(600, 60 ));
    p8.push_back(Vector2dd(660, 540));
    l8 = {{1}, {2}, {1}};
    s8.space = p8;
    s8.linedefs = l8;

    level.sectors = {s1, s2, s3, s4, s5, s6, s7, s8};
    for (size_t i = 0; i < level.sectors.size(); i++)
    {
        level.sectors[i].floorColor = RGBColor::rainbow(((double)i / level.sectors.size()));
    }
//    level.sectors = {s4, s5};

    BSPRenderer::DrawLevel(level, buffer, true);
    BMPLoader().save("level.bmp", &buffer);

    buffer.fillWith(RGBColor::Black());
    tree = BSPRenderer::BSPNode2d(level, 0, 0);
    BSPRenderer::DrawBSPNode(tree, buffer);
//    BMPLoader().save(".bmp", &buffer);
}

TEST(BSPRender, levelDraw)
{
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::White());
    Polygon p;

    p.push_back(Vector2dd(350, 350));
    p.push_back(Vector2dd(370, 650));
    p.push_back(Vector2dd(400, 670));
    p.push_back(Vector2dd(510, 350));

//    p.push_back(Vector2dd(59, 59));
//    p.push_back(Vector2dd(59, 659));

//    p.push_back(Vector2dd(299, 599));
//    p.push_back(Vector2dd(299, 419));
//    p.push_back(Vector2dd(479, 479));
//    p.push_back(Vector2dd(479, 659));

//    p.push_back(Vector2dd(539, 659));
//    p.push_back(Vector2dd(659, 479));
//    p.push_back(Vector2dd(659, 59));

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

    Ray2d ray = Ray2d::FromPoints(Vector2dd(100, 600), Vector2dd(350, 350));
    Vector2dd p1 = ray.getPoint(0.0);
    Vector2dd p2 = ray.getPoint(8.0);
    buffer->drawLine(p1, p2, RGBColor::Yellow());
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

//    for (int i = 1; i < 10; i++)
//    {
//        Ray2d ray( Vector2dd::FromPolar( M_PI / 20.0 * i + 0.02, 100), Vector2dd(140,180));

//        Vector2dd p1 = ray.getPoint(0.0);
//        Vector2dd p2 = ray.getPoint(8.0);
//        buffer->drawLine(p1, p2, RGBColor::Yellow());

//        for (int j = 0; j < 8; j++)
//        {
//            Vector2dd p = ray.getPoint(j);
//            buffer->drawCrosshare1(p.x(), p.y(), RGBColor::Cyan());
//        }

//        double t1, t2;
//        ray.clip<Polygon>(p, t1, t2);

//        p1 = ray.getPoint(t1);
//        p2 = ray.getPoint(t2);

//        cout << "t1 = " << t1 << endl;
//        cout << "t2 = " << t2 << endl;
//        cout << "p1 = " << p1 << endl;
//        cout << "p2 = " << p2 << endl;

//        buffer->drawLine(p1, p2,
//                       t1 > t2 ? RGBColor::Red() : RGBColor::Green());

//    }

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

TEST(BSPRender, test)
{
    int h = 720;
    int w = 720;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::White());

    Segment2d seg(Vector2dd(60, 240), Vector2dd(240, 120));
    Vector2dd point(150, 180);
    Line2d line(seg);

    BSPRenderer::OnSegment(seg, point);
////    cout << "--- " << line.side(Vector2dd(400, 359)) << " "
////                   << line.side(Vector2dd(400, 361)) << endl
//    cout << "seg: " << seg << " , point: " << point
//         << " , on: " << BSPRenderer::OnSegment(seg, point)
//         << endl;
}
