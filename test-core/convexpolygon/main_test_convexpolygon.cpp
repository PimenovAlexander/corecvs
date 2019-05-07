/**
 * \file main_test_convexpolygon.cpp
 * \brief This is the main file for the test convexpolygon 
 *
 * \date апр 28, 2019
 * \author alexander
 *
 * \ingroup autotest  
 */

#include <iostream>
#include <core/buffers/bufferFactory.h>
#include <core/buffers/rgb24/abstractPainter.h>
#include <core/geometry/mesh3d.h>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/geometry/convexPolyhedron.h"


#include "core/geometry/convexHull.h"


using namespace std;
using namespace corecvs;


void drawPolygon2Color(RGB24Buffer *target, const Polygon &p, RGBColor color1, RGBColor color2)
{
    if (p.empty())
        return;

    if (p.size() == 1)
    {
        target->drawLine(p[0].x(), p[0].y(), p[0].x(), p[0].y(), color1);
    }
    for (unsigned i = 0; i < p.size(); i++)
    {
        Vector2dd p1 = p.getPoint    (i);
        Vector2dd p2 = p.getNextPoint(i);
        Vector2dd p =  (p1 + p2) / 2.0;

        target->drawLine(p1, p , color1);
        target->drawLine(p , p2, color2);
    }
}

void drawConvexPolygon(RGB24Buffer *target, const ConvexPolygon &cp, RGBColor color)
{
    AbstractPainter<RGB24Buffer> painter(target);
    for (size_t i = 0; i < cp.faces.size(); i++)
    {
        const Line2d &l = cp.faces[i];
        painter.drawLine2d(l, RGBColor::parula((double)i / cp.faces.size()), 5.0);
    }
}

/*
void storeDualAsMesh(const ConvexPolygon &cp, )
{
    AbstractPainter<RGB24Buffer> painter(target);
    for (size_t i = 0; i < cp.faces.size(); i++)
    {
        const Line2d &l = cp.faces[i];
        painter.drawLine2d(l, RGBColor::parula((double)i / cp.faces.size()), 5.0);
    }
}
*/


/**

  *----        ####*
  |    ----####    #
  |    ####----    #
  *####        ----*
  |    ####----    #
  |    ----####    #
  *----        ####*

 **/

TEST(convexPolygon, testIntersection)
{
    RGB24Buffer buffer(330, 330);

    Vector2dd shifts[3] = {
        Vector2dd(-100, -100),
        Vector2dd(   0,    0),
        Vector2dd( 100,  100)
    };

    Vector2dd drawShift = Vector2dd( 100,  100);

    for (size_t i = 0; i < CORE_COUNT_OF(shifts); i++)
    {

        Polygon p1;
        p1.push_back(Vector2dd( 10,  10));
        p1.push_back(Vector2dd( 10, 100));
        p1.push_back(Vector2dd( 99,  60));

        Polygon p2;
        p2.push_back(Vector2dd(100, 100));
        p2.push_back(Vector2dd(100,  10));
        p2.push_back(Vector2dd( 11,  60));

        /* Move poligons to interesting positions  */
        Polygon pt1 = p1.translated(shifts[i]);
        Polygon pt2 = p2.translated(shifts[i]);

        ConvexPolygon cp1 = pt1.toConvexPolygon();
        ConvexPolygon cp2 = pt2.toConvexPolygon();

        ConvexPolygon cp3 = ConvexPolygon::intersect(cp1, cp2);

        Polygon p3;
        p3.reserve(cp3.faces.size());
        for (size_t i = 0; i < cp3.faces.size(); i++)
        {
            Line2d &l1 = cp3.faces[i];
            Line2d &l2 = cp3.faces[(i + 1) % cp3.faces.size()];
            p3.push_back(l1.intersectWith(l2));
        }

        cout << "cp1" << p1 << endl;
        cout << "cp2" << p2 << endl;

        cout << "result" << p3 << endl;

        drawPolygon2Color(&buffer, pt1.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
        drawPolygon2Color(&buffer, pt2.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
        drawPolygon2Color(&buffer, p3 .translated(drawShift), RGBColor::White(), RGBColor::Cyan());

        Polygon bp3 = p3.translated(-shifts[i]);
        CORE_ASSERT_TRUE(bp3.size() == 4, "Wrong intersect size");

        CORE_ASSERT_TRUE(bp3.getPoint(0).notTooFar(Vector2dd(99, 60    ), 0.1), "Wrong intersect point 0");
        CORE_ASSERT_TRUE(bp3.getPoint(1).notTooFar(Vector2dd(55, 79.775), 0.1), "Wrong intersect point 1");
        CORE_ASSERT_TRUE(bp3.getPoint(2).notTooFar(Vector2dd(11, 60    ), 0.1), "Wrong intersect point 2");
        CORE_ASSERT_TRUE(bp3.getPoint(3).notTooFar(Vector2dd(55, 35.280), 0.1), "Wrong intersect point 3");
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "non_eigen1.bmp");

}

TEST(convexPolygon, testConvexToPolygon)
{
    Polygon p = {
        Vector2dd(10,10),
        Vector2dd(10,20),
        Vector2dd(30,20),
        Vector2dd(30,10)
    };

    bool orientation = true;
    bool isConvex = p.isConvex(&orientation);
    CORE_ASSERT_TRUE(isConvex, "Convex error");
    CORE_ASSERT_TRUE(orientation == false, "Orientation error");
    if (!orientation) {
        p.reverse();
    }
    ConvexPolygon cp = p.toConvexPolygon();
    Vector2dd point(20,15);
    cout << cp.isInside(point) << endl;
    Polygon pr = Polygon::FromConvexPolygon(cp);
    cout << p  << endl;
    cout << pr << endl;
}

TEST(convexPolygon, testDuality)
{
    Polygon p1 = Polygon::RegularPolygon(4, Vector2dd(0.0, 0.0), 10.0, 0.0);
    ConvexPolygon cp1 = p1.toConvexPolygon();
    cout << "Convex polygon1" << cp1 << endl;

    Polygon p2 = Polygon::RegularPolygon(4, Vector2dd(20.0, 20.0), 10.0, 0.0);
    ConvexPolygon cp2 = p2.toConvexPolygon();
    cout << "Convex polygon2" << cp2 << endl;
}


void drawProjectiveSpace(vector<ConvexPolygon> cp, const string &name )
{
    RGB24Buffer projective(1000,1000);
    Rectangled  rect = Rectangled::Empty();

    AbstractPainter<RGB24Buffer> ppainter(&projective);

    vector<Polygon> dp;
    dp.resize(cp.size());

    for (size_t i = 0; i < cp.size(); i++)
    {
        cout << cp[i] << endl;

        for (size_t j = 0; j < cp[i].faces.size(); j++)
        {
            Vector2dd proj = cp[i].faces[j].project();
            dp[i].push_back(proj);
            cout << proj << " ";
            rect.extendToFit(proj);
        }
        cout << endl;
    }
    cout << endl << "Fit area" << rect << endl;

    Matrix33 rescale =
            Matrix33::Scale2   (
                (projective.w - 3) / rect.size.x(),
                (projective.h - 3) / rect.size.y()
            ) * Matrix33::ShiftProj( -rect.corner);


    cout << "Redraw" << endl << rescale << endl;
    for (size_t i = 0; i < cp.size(); i++)
    {
        RGBColor color = (i == 0) ? RGBColor::Red() : RGBColor::parula(i / 10.0);

        for (size_t j = 0; j < cp[i].faces.size(); j++)
        {
            Vector2dd proj = cp[i].faces[j].project();
            projective.drawCrosshare3(rescale * proj, color);
            cout  << rescale * proj << endl;
        }
        cout << endl;
        dp[i].transform(rescale);
        ppainter.drawPolygon(dp[i], color);
    }

//    BufferFactory::getInstance()->saveRGB24Bitmap(space     , name + "_poly.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(projective, name + "_proj.bmp");

}

TEST(convexPolygon, movePolygon)
{
    vector<Polygon> p;
    vector<ConvexPolygon> cp;

    RGB24Buffer space(500,500);
    AbstractPainter<RGB24Buffer> spainter(&space);


    Polygon p1 = Polygon::RegularPolygon(4, Vector2dd(300, 300), 100.0, degToRad(30.0));
    cp.push_back(p1.toConvexPolygon());
    spainter.drawPolygon(p1, RGBColor::Red());

    for (int i = 0; i < 10; i++)
    {
        double pos = lerp(20, 400, i, 0.0, 10.0);
        p.push_back(Polygon::RegularPolygon(4, Vector2dd(pos, pos), 10.0, degToRad(30.0)));
        cp.push_back(p[i].toConvexPolygon());
        //cp[i].append(p1.toConvexPolygon());
        spainter.drawPolygon(p[i], RGBColor::Green());
    }

    drawProjectiveSpace(cp, string("move"));
    BufferFactory::getInstance()->saveRGB24Bitmap(space, "move_poly.bmp");
}

TEST(convexPolygon, testCut)
{
    RGB24Buffer space(50,50);
    AbstractPainter<RGB24Buffer> painter(&space);

    Polygon p = {
        Vector2dd(10,10),
        Vector2dd(10,20),
        Vector2dd(30,20),
        Vector2dd(30,10)
    };
    p.reverse();
    painter.drawPolygon(p, RGBColor::Gray());


    ConvexPolygon cp = p.toConvexPolygon();
    /*ConvexPolygon cp1 = cp;
    cp1.simplify();*/
    Polygon pr = Polygon::FromConvexPolygon(cp);
    painter.drawPolygon(pr, RGBColor::Purple());
    cout << "Input polygon:" << endl;
    cout << p  << endl;
    cout << pr << endl;


    ConvexPolygon cp2 = cp;
    cp2.faces.push_back(Line2d(Vector2dd(20,10), Vector2dd(20,20)));
    cout << "In 2:" << cp2 << endl;
    cp2.simplify();
    cout << "Cut2:" << cp2 << endl;
    Polygon p2 = Polygon::FromConvexPolygon(cp2);
    painter.drawPolygon(p2, RGBColor::Red());

    ConvexPolygon cp3 = cp;
    cp3.faces.push_back(Line2d(Vector2dd(20,20), Vector2dd(20,10)));
    cout << "In 3:" << cp3 << endl;
    cp3.simplify();
    cout << "Cut3:" << cp3 << endl;
    Polygon p3 = Polygon::FromConvexPolygon(cp3);
    painter.drawPolygon(p3, RGBColor::Green());

    Vector2dd point2(15,15);
    Vector2dd point3(25,15);
    space.drawCrosshare3(point2, RGBColor::Cyan());
    space.drawCrosshare3(point3, RGBColor::Yellow());

    BufferFactory::getInstance()->saveRGB24Bitmap(space     , "divide_poly.bmp");
    CORE_ASSERT_TRUE(cp2.isInside(point2), "Wrong cut 1");
    CORE_ASSERT_TRUE(cp3.isInside(point3), "Wrong cut 2");

    vector<ConvexPolygon> cpl = {cp2, cp3};
    drawProjectiveSpace(cpl, "dual");
    {
        ConvexPolygon cp3 = cp;
        cp3.faces.push_back(Line2d(Vector2dd(20,20), Vector2dd(20,10)));
        ConvexPolygon cp2 = cp;
        cp2.faces.push_back(Line2d(Vector2dd(20,10), Vector2dd(20,20)));
        vector<ConvexPolygon> cpl = {cp2, cp3};
        drawProjectiveSpace(cpl, "dual1");
    }
}


/** Empty intersection **/
TEST(convexPolygon, testEmpty)
{
    RGB24Buffer space1(100, 100);
    RGB24Buffer space2(100, 100);

    Polygon p1 = Polygon::RegularPolygon(4, Vector2dd(10.0, 10.0), 10.0, 0.0);
    Polygon p2 = Polygon::RegularPolygon(4, Vector2dd(40.0, 10.0), 10.0, 0.0);

    ConvexPolygon cp1 = p1.toConvexPolygon();
    ConvexPolygon cp2 = p2.toConvexPolygon();

    ConvexPolygon u = ConvexPolygon::merge(cp1, cp2);
    cout << "United\n" <<  u << endl;
    drawConvexPolygon(&space1, u, RGBColor::Red());
    ConvexPolygon s = u;
    s.simplify();
    cout << "Simplifed\n" <<  s << endl;
    drawConvexPolygon(&space2, s, RGBColor::Green());


    BufferFactory::getInstance()->saveRGB24Bitmap(space1, "empty1_poly.bmp");
    BufferFactory::getInstance()->saveRGB24Bitmap(space2, "empty2_poly.bmp");

    ConvexPolygon cp3 = ConvexPolygon::intersect(cp1, cp2);

     /** **/
     {
        vector<Vector3dd> dual;
        dual.reserve(cp1.size() + cp2.size());
        for (const Line2d &line : cp1.faces)
        {
            dual.push_back(line.toDualP());
            cout << "D1: " << line.toDualP() << endl;
        }
        for (const Line2d &line : cp2.faces)
        {
            dual.push_back(line.toDualP());
            cout << "D2: " << line.toDualP() << endl;
        }

     }

     cout << "p1" << p1 << endl;
     cout << "p2" << p2 << endl;

     cout << "CP result" << cp3 << endl;

     Polygon p3 = Polygon::FromConvexPolygon(cp3);

     cout << "P result" << p3 << endl;

     //CORE_ASSERT_TRUE(cp3.size() == 0, "Wrong intersect size");
}


TEST(convexPolygon, fromRectangle)
{
    Rectangled r(Vector2dd(10,10), Vector2dd(10,10));
    ConvexPolygon cp = r.toConvexPolygon();
    CORE_ASSERT_TRUE( cp.isInside(Vector2dd(15,15)), "Rect to convex failed");
    CORE_ASSERT_TRUE(!cp.isInside(Vector2dd( 5, 5)), "Rect to convex failed");
}

TEST(convexPolygon, drawConvex)
{
    RGB24Buffer space(100, 100);
    Polygon p = Polygon::RegularPolygon(5, Vector2dd(50,50), 30, degToRad(50));
    ConvexPolygon cp = p.toConvexPolygon();
    drawConvexPolygon(&space, cp, RGBColor::Green());
    BufferFactory::getInstance()->saveRGB24Bitmap(space, "convex_poly.bmp");
}


/* Temporary function */
ProjectivePolygon GiftWrap(std::vector<Vector3dd> points)
{
    ProjectivePolygon toReturn;
    if (points.empty()) {
        return toReturn;
    }
    if (points.size() == 1) {
        toReturn.push_back(points[0]);
        return toReturn;
    }

    /* Find one point in hull */
    size_t iMin = 0;
    double yMin = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); i++)
    {
        double ycost = points[i].normalised() & Vector3dd::OrtZ();
        if (ycost < yMin) {
            yMin = ycost;
            iMin = i;
        }
    }

#if 0
    // SYNC_PRINT(("ConvexHull::GiftWrap(): starting with point %i (%lf %lf)\n", minYId, list[minYId].x(), list[minYId].y() ));
    toReturn.push_back(points[iMin]);

    Vector3dd prev = Vector3dd::OrtX();
    Vector3dd current = points[iMin];

    /* Wrap */
    do {
        Vector3dd next;
        Vector3dd nextDir(0.0);
        double vmin = std::numeric_limits<double>::max();

        for (const Vector3dd &point : points)
        {
            if (point == current) {
                continue;
            }

            //Vector2dd dir1 = (point - current).normalised();
            //double v = direction & dir1;
            double v = ccwProjective(direction, current, point);
            if (v < vmin) {
                vmin = v;
                next = point;
                nextDir = point.;
            }
        }

        if (next == toReturn.front())
        {
            break;
        }
        toReturn.push_back(next);
        current = next;
        direction = nextDir;

    } while (true);
    return toReturn;
#endif
}


TEST(convexPolygon, convexStudies)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    mesh.addOrts(10.0);

    Vector2dd center1( 10, 10);
    Vector2dd center2(-10,-10);

    ConvexPolygon cp;
    for (int i = 0; i < 360; i++)
    {
        double phi = degToRad(i);
        Vector2dd d1 = Vector2dd::FromPolar(phi, 2.0);
        Vector2dd d2 = Vector2dd::FromPolar(phi, 4.0);

        Vector2dd point1 = center1 + d1;
        Line2d l1 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point1, d1.leftNormal()));
        Vector3dd dual1 = l1.toDualP();
        mesh.setColor(RGBColor::Red());
        mesh.addPoint(Vector3dd(point1, 1.0));
        mesh.addPoint(dual1.normalised());
        cp.faces.push_back(l1);

        Vector2dd point2 = center1 + d2;
        Line2d l2 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point2, d2.leftNormal()));
        Vector3dd dual2 = l2.toDualP();
        mesh.setColor(RGBColor::Green());
        mesh.addPoint(Vector3dd(point2, 1.0));
        mesh.addPoint(dual2.normalised());
        cp.faces.push_back(l2);

        Vector2dd point3 = center2 + d1;
        Line2d l3 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point3, d1.leftNormal()));
        Vector3dd dual3 = l3.toDualP();
        mesh.setColor(RGBColor::Blue());
        mesh.addPoint(Vector3dd(point3, 1.0));
        mesh.addPoint(dual3.normalised());
        //cp.faces.push_back(l3);
    }

    vector<Vector3dd> points = cp.toDualPoints();
//    cp.simplify();

    ProjectivePolygon pol = GiftWrap(points);

    for (size_t i = 0; i < pol.size(); i++)
    {
        mesh.setColor(RGBColor::Magenta());
        mesh.addPoint(pol[i].normalised() + Vector3dd(0.1));
    }

#if 0
    Polygon p = Polygon::FromConvexPolygon(cp);
    for (int i = 0; i < p.size(); i++)
    {
        mesh.setColor(RGBColor::Magenta());
        mesh.addPoint(Vector3dd(p[i], 1.1));
    }
#endif

    mesh.dumpPLY("dual.ply");
}
