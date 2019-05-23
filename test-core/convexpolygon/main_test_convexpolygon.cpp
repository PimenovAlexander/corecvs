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

#include "convexDebug.h"


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

void drawConvexPolygon(RGB24Buffer *target, const ConvexPolygon &cp, RGBColor /*color*/)
{
    AbstractPainter<RGB24Buffer> painter(target);
    for (size_t i = 0; i < cp.faces.size(); i++)
    {
        const Line2d &l = cp.faces[i];
        painter.drawLine2d(l, RGBColor::parula((double)i / cp.faces.size()), 5.0);
    }
}

TEST(convexPolygon, testCSubdiv)
{

    RGB24Buffer buffer(630, 630);

    Polygon p1;
    p1.push_back(Vector2dd( 220,  220));
    p1.push_back(Vector2dd( 220,  400));
    p1.push_back(Vector2dd( 398,  320));

    p1.reverse();
    ConvexPolygon cp1 = p1.toConvexPolygonSubdivide(5);

    for ( int i = 0; i < buffer.h; i++)
    {
        for ( int j = 0; j < buffer.w; j++)
        {
            buffer.element(i,j) = cp1.isInside(Vector2dd(j,i)) ?
                        RGBColor::Gray() : RGBColor::Navy();
        }
    }

    drawConvexPolygon(&buffer, cp1, RGBColor::Red());

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "convex-subdiv.bmp");
}

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

    Vector2dd shifts[] = {
        Vector2dd(-100, -100), // 0
        Vector2dd(-100,  100), // 1
        Vector2dd(   0,    0), // 2
        Vector2dd( 100,  100), // 3
        Vector2dd( 100, -100)  // 4

    };

    Polygon p1 = {
        { 10,  10 },
        { 10, 100 },
        { 99,  60 }
    };

    Polygon p2 = {
        {100, 100},
        {100,  10},
        { 11,  60}
    };

    Vector2dd check(50, 50);

    bool trace = false;

    Vector2dd drawShift = Vector2dd( 100,  100);

    for (int rotation = 1; rotation < 360; rotation++)
    {
        for (size_t i = 1; i < CORE_COUNT_OF(shifts); i++)
        {
            cout << "#";
            Matrix33 transform = Matrix33::ShiftProj(shifts[i]) * Matrix33::RotationZ(degToRad(rotation));
            /* Move poligons to interesting positions  */
            Polygon pt1 = Polygon::Reversed(p1.transformed(transform));
            Polygon pt2 = Polygon::Reversed(p2.transformed(transform));

            ConvexPolygon cp1 = pt1.toConvexPolygon();
            ConvexPolygon cp2 = pt2.toConvexPolygon();


            CORE_ASSERT_TRUE(cp1.isInside(transform * check), "Wrong orientation");
            CORE_ASSERT_TRUE(cp2.isInside(transform * check), "Wrong orientation");

#if 0
            Mesh3D mesh;
            mesh.switchColor();
            ConvexPolygon cp3 = ConvexPolygon::merge(cp1, cp2);
            ConvexPolygon pcp3 = cp3.permutate(rotation + i * 360);
            ProjectivePolygon pp = ProjectivePolygon::FromConvexPolygon(pcp3);
            ProjectivePolygon pps;
            bool result = ConvexDebug::GiftWrap(pp, pps, &mesh);
            cout << "Intersect Result:" << result << endl;
            cp3 = pps.toConvexPolygon();
            mesh.dumpPLY("intersect1.ply");
#else
         ConvexPolygon cp3 = ConvexPolygon::intersect(cp1, cp2);
#endif


            Polygon p3;
            p3.reserve(cp3.faces.size());
            for (size_t i = 0; i < cp3.faces.size(); i++)
            {
                Line2d &l1 = cp3.faces[i];
                Line2d &l2 = cp3.faces[(i + 1) % cp3.faces.size()];
                p3.push_back(l1.intersectWith(l2));
            }

            drawPolygon2Color(&buffer, pt1.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
            drawPolygon2Color(&buffer, pt2.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
            drawPolygon2Color(&buffer, p3 .translated(drawShift), RGBColor::White(), RGBColor::Cyan());

            Polygon bp3 = p3.transformed(transform.inv());

#if 1
            if (bp3.size() != 4 || trace) {
                cout << "Rotation:" << rotation << endl;
                cout << "Shift:" << i << "  " << shifts[i] << endl;
                cout << "Matrix:\n" << transform << endl;

                cout << "pt1" << pt1 << endl;
                cout << "pt2" << pt2 << endl;
                cout << "result" << bp3 << endl;
            }

#else

            CORE_ASSERT_TRUE_P(bp3.size() == 4, ("Wrong intersect size %d != 4\n", bp3.size()));

            CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(99, 60    ), 0.1), "Wrong intersect point 0");
            CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(55, 79.775), 0.1), "Wrong intersect point 1");
            CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(11, 60    ), 0.1), "Wrong intersect point 2");
            CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(55, 35.280), 0.1), "Wrong intersect point 3");
#endif
       }
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "non_eigen1.bmp");

}

TEST(convexPolygon, testProject)
{
    vector<Line2d>    lines = {
        Line2d(Vector2dd(0,-5), Vector2dd(-1,0)),
        Line2d(Vector2dd(0,-1), Vector2dd(-1,0)),
        Line2d(Vector2dd(0, 1), Vector2dd( 1,0)),
        Line2d(0.739214, 5.05035, -36.2807)
    };

    vector<Vector2dd> points = {
        Vector2dd( 0,0),
        Vector2dd( 0,0),
        Vector2dd(-1,0),
        Vector2dd( 0,0)
    };


    for (int i = 0; i < points.size(); i++)
    {
        Line2d line = lines[i];
        Vector2dd point = points[i];
        Vector2dd res1 = line.projectPointTo(point);
        cout << "Result 1" <<  res1 << endl;

        Vector3dd lp = line.toDualP();
        Vector3dd pp = Vector3dd(point, 1.0);

        Vector3dd n = Vector3dd((lp.z() > 0) ? lp.xy() : -lp.xy(), 0.0);
        //Vector3dd n = lp;
        //Vector3dd proj = n ^ lp;
        Vector3dd res2 = (n ^ pp) ^ lp;
        cout << "Result 2" <<  res2.project() << endl;

        Vector3dd res3 = ConvexHull::project(lp, pp);
        cout << "Result 3" <<  res3.project() << endl;


        CORE_ASSERT_TRUE(res2.project().notTooFar(res1), "Wrong projective project");
    }
}


TEST(convexPolygon, testInitialPoint)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    mesh.setColor(RGBColor::Red());

    Polygon p1 = {{ 10, 100}, { 10,  10}, { 99,  60}};
    Polygon p2 = {{100,  10}, {100, 100}, { 11,  60}};

    int rotation = 21;
    Vector2dd shift(-100, 100);

    Matrix33 transform = Matrix33::ScaleProj(1 / 20.0) * Matrix33::ShiftProj(shift) * Matrix33::RotationZ(degToRad(rotation));
    p1.transform(transform);
    p2.transform(transform);

    mesh.setColor(RGBColor::Red());
    for (size_t i = 0; i < p1.size(); i++) {
        mesh.addLine(Vector3dd(p1.getPoint(i), 0.9), Vector3dd(p1.getNextPoint(i), 0.9));
    }
    mesh.setColor(RGBColor::Blue());
    for (size_t i = 0; i < p2.size(); i++) {
        mesh.addLine(Vector3dd(p2.getPoint(i), 0.9), Vector3dd(p2.getNextPoint(i), 0.9));
    }

    Rectangled holder = Rectangled::Empty();
    holder.extendToFit(p1);
    holder.extendToFit(p2);

    ConvexPolygon cp3 = ConvexPolygon::merge(p1.toConvexPolygon(), p2.toConvexPolygon());

    //23
    for (int i = 0; i < 1000; i++) {
        ConvexPolygon pcp3 = cp3.permutate(i);
        ProjectivePolygon points = ProjectivePolygon::FromConvexPolygon(pcp3);
        cout << "Fail Mode:\n" << points << endl;
        int id = ConvexDebug::InitailPoint(points, &mesh);

        mesh.setColor(RGBColor::Amber());
        ConvexDebug::drawProjectiveLines(mesh, points[id], Circle2d(holder.center(), holder.size.l2Metric()));
    }

    mesh.dumpPLY("initial.ply");

}

TEST(convexPolygon, testInterDraw)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    mesh.setColor(RGBColor::Red());
    RGB24Buffer buffer(330, 330);

    Polygon p1 = {
        { 10,  10 },
        { 10, 100 },
        { 99,  60 }
    };

    Polygon p2 = {
        {100, 100},
        {100,  10},
        { 11,  60}
    };

    int rotation = 21;
    Vector2dd shift(-100, 100);

    Matrix33 transform = Matrix33::ScaleProj(1/10.0) * Matrix33::ShiftProj(shift) * Matrix33::RotationZ(degToRad(rotation));

    p1.transform(transform);
    p2.transform(transform);

    Rectangled holder = Rectangled::Empty();
    holder.extendToFit(p1);
    holder.extendToFit(p2);

    /* Move poligons to interesting positions  */
    Polygon pt1 = p1; pt1.reverse();
    Polygon pt2 = p2; pt2.reverse();

    mesh.setColor(RGBColor::Red());
    for (size_t i = 0; i < pt1.size(); i++)
    {
        mesh.addLine(Vector3dd(pt1.getPoint(i), 0.9), Vector3dd(pt1.getNextPoint(i), 0.9));
    }

    mesh.setColor(RGBColor::Blue());
    for (size_t i = 0; i < pt2.size(); i++)
    {
        mesh.addLine(Vector3dd(pt2.getPoint(i), 0.9), Vector3dd(pt2.getNextPoint(i), 0.9));
    }


    ConvexPolygon cp1 = pt1.toConvexPolygon(); //Subdivide(200);
    ConvexPolygon cp2 = pt2.toConvexPolygon(); //Subdivide(200);
    //ConvexPolygon cp1 = pt1.toConvexPolygonSubdivide(40);
    //ConvexPolygon cp2 = pt2.toConvexPolygonSubdivide(40);

    cout << "Check" << endl;
    Vector2dd check(5.0, 5.0);
    cout << cp1.isInside(check) << endl;
    cout << cp2.isInside(check) << endl;

    ConvexPolygon cp3 = ConvexPolygon::merge(cp1, cp2);

    for (Line2d &line : cp3.faces)
    {
    //    mesh.addPoint(Vector3dd(line.toDualS(), 0.5));
        mesh.addPoint(line.toDualP().normalised());
    }

    //cp3.simplify();
    ProjectivePolygon points = ProjectivePolygon::FromConvexPolygon(cp3);
    ProjectivePolygon simplified;
    bool result = ConvexDebug::GiftWrap(points, simplified, &mesh);
    //bool result = ConvexHull::GiftWrap(points, simplified, &mesh);


    cout << "Result: " << (result ? "true" : "false") << endl;

#if 1

    mesh.setColor(RGBColor::Green());
    Circle2d circle2d(holder.center(), holder.size.l2Metric());
    for (Vector3dd p : simplified) {
        ConvexDebug::drawProjectiveLines(mesh, p, circle2d);
    }

#endif




#if 0
    Polygon p3;
    p3.reserve(cp3.faces.size());
    for (size_t i = 0; i < cp3.faces.size(); i++)
    {
        Line2d &l1 = cp3.faces[i];
        Line2d &l2 = cp3.faces[(i + 1) % cp3.faces.size()];
        Vector2dd point = l1.intersectWith(l2);

        if (!p3.empty() && p3.back().notTooFar(point, 1e-5))
            continue;
        p3.push_back(point);
    }

    for (size_t i = 0; i < p3.size(); i++)
    {
        mesh.addLine(Vector3dd(p3.getPoint(i), 1.0), Vector3dd(p3.getNextPoint(i), 1.01));
    }
#endif

    mesh.dumpPLY("intersect.ply");
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

    ASSERT_TRUE((s.faces.empty(), "Failed to intersect disjoint polygons"));

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





Vector3dd st1(Line2d &input)
{
    return Vector3dd(input.toDualP().project(), 0.1);
}

Vector3dd st(Line2d &input)
{
    return input.toDualP().normalised();
}


Vector3dd st3(Line2d &input)
{
    Vector3dd t = input.toDualP().normalised();
    Vector3dd sg = t + Vector3dd::OrtZ();

    return sg / sg.z();
}

Vector3dd st4(Line2d &input)
{
    return Vector3dd(input.toDualS(), 0.5);
}

TEST(convexPolygon, convexStudies)
{
    Mesh3D mesh;
    mesh.switchColor(true);
    mesh.addOrts(10.0);

    Vector2dd center1( 4, 4);
    Vector2dd center2(-4,-4);

    ConvexPolygon cp;
    for (int i = 0; i < 360; i++)
    {
        double phi = degToRad(i);
        Vector2dd d1 = Vector2dd::FromPolar(phi, 2.0);
        Vector2dd d2 = Vector2dd::FromPolar(phi, 4.0);

        Vector2dd point1 = center1 + d1;
        Line2d l1 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point1, d1.leftNormal()));
        mesh.setColor(l1.z() > 0 ? RGBColor::Red() : (RGBColor::Red() / 2) );
        mesh.addPoint(Vector3dd(point1, 1.0));
        mesh.addPoint(st(l1));
        mesh.addPoint(st3(l1));
//        mesh.addPoint(st4(l1));
        cp.faces.push_back(l1);

        Vector2dd point2 = center1 + d2;
        Line2d l2 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point2, d2.leftNormal()));
        mesh.setColor(l2.z() > 0 ? RGBColor::Green() : (RGBColor::Green() / 2) );
        mesh.addPoint(Vector3dd(point2, 1.0));
        mesh.addPoint(st(l2));
        mesh.addPoint(st3(l2));
//        mesh.addPoint(st4(l2));
        cp.faces.push_back(l2);

#if 0
        Vector2dd point3 = center2 + d1;
        Line2d l3 = Line2d::FromRay(Ray2d::FromOriginAndDirection(point3, d1.leftNormal()));
        mesh.setColor(l3.z() > 0 ? RGBColor::Blue() : (RGBColor::Blue() / 2) );
        mesh.addPoint(Vector3dd(point3, 1.0));
        mesh.addPoint(st(l3));
        mesh.addPoint(st3(l3));
//        mesh.addPoint(st4(l3));
       cp.faces.push_back(l3);
#endif
    }

#if 1
    ProjectivePolygon points = ProjectivePolygon::FromConvexPolygon(cp);
//    cp.simplify();

    ProjectivePolygon simplified;
    bool result = ConvexDebug::GiftWrap(points, simplified, &mesh);

#if 0
    for (size_t i = 0; i < pol.size(); i++)
    {
        mesh.setColor(RGBColor::Magenta());
        mesh.addIcoSphere(pol[i].normalised(), 0.001);
        //mesh.addIcoSphere(Vector3dd(pol[i].project(), 1.0), 0.005);
        if (i != pol.size() - 1) {
            mesh.addLine(pol[i].normalised(), pol[i + 1].normalised());
        }
    }
#endif
#endif


#if 0
    cp.simplify();
    for (Line2d &l : cp.faces) {
        mesh.setColor(RGBColor::Yellow());
        mesh.addPoint(st4(l) + Vector3dd(0, 0, 0.75));
    }

    Polygon p = Polygon::FromConvexPolygon(cp);
    for (int i = 0; i < p.size(); i++)
    {
        mesh.setColor(RGBColor::Magenta());
        mesh.addPoint(Vector3dd(p[i], 1.1));
    }
#endif

    mesh.dumpPLY("dual.ply");
}
