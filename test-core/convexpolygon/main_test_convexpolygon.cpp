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

/* Temporary function */
bool GiftWrap(ProjectivePolygon &points, ProjectivePolygon &output, Mesh3D *debug)
{
    output.clear();

    if (points.empty()) {
        return false;
    }
    if (points.size() == 1) {
        output.push_back(points[0]);
        return true;
    }

#if 1
    /* Find one point in hull */
    size_t iMin = 0;
    double zMin = std::numeric_limits<double>::max();

    for (size_t i = 0; i < points.size(); i++)
    {
        double zcost = points[i].normalised() & Vector3dd::OrtZ();
        if (zcost < zMin) {
            zMin = zcost;
            iMin = i;
        }
    }
    if (debug != NULL) debug->setColor(RGBColor::Red());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);


    /* We have now selected direction let's check if we have lines futher away from  */
    Vector3dd orth  = Vector3dd::OrtZ() ^ points[iMin];
    Vector3dd plane = points[iMin] ^ orth; /* in euclidean coordinates this is a point */

    double cMin = std::numeric_limits<double>::max();
    for (size_t i = 0; i < points.size(); i++)
    {
        double dot = plane & points[i].normalised();
        if (dot < cMin)
        {
            cMin = dot;
            iMin = i;
        }
    }
    if (debug != NULL) debug->setColor(RGBColor::Yellow());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);

    /* We now have a direction to work from */

    output.push_back(points[iMin]);
    Vector3dd prev = points[iMin];

    SYNC_PRINT(("ConvexHull::GiftWrap(): starting with point (%d of %d) (%lf %lf %lf)\n",
                (int)iMin, (int)points.size(),
                prev.x(), prev.y(), prev.z()));
    if (debug != NULL) debug->setColor(RGBColor::Amber());
    if (debug != NULL) debug->addIcoSphere(points[iMin].normalised(), 0.001);


    /* Find second point in hull. A known interior point could save time here */
    int sId = -1;
    for (size_t i = 0; i < points.size(); i++)
    {
        if (i == iMin) {
            continue;
        }
        Vector3dd normal = prev ^ points[i];
        cout << " " << i << " p:" << points[i] << " - " << normal << endl;

        size_t j = 0;
        for (; j < points.size(); j++)
        {
            cout << "   " << j;
            if (j == i || j == iMin) {
                cout << " skip" << endl;
                continue;
            }
            double v = (normal & points[j]);
            cout << "  " << v << endl;
            if (v < 0) {
                break;
            }
        }
        if (j == points.size()) {
            sId = i;
            break;
        }
    }
    if (sId == -1) {
        return false;
    }

    Vector3dd current = points[sId];
    output.push_back(points[sId]);
    SYNC_PRINT(("ConvexHull::GiftWrap(): second point (%d of %d) (%lf %lf %lf)\n",
                (int)iMin, (int)points.size(),
                current.x(), current.y(), current.z()));
#endif

#if 0
    int mini = 0;
    int minj = 1;
    double zMin = std::numeric_limits<double>::max   ();
    double zMax = std::numeric_limits<double>::lowest();

    for (int i = 0; i < points.size(); i++)
    {
        for (int j = 0; j < points.size(); j++)
        {
            if (i == j) continue;
            Vector3dd p1 = points[i].normalised();
            Vector3dd p2 = points[j].normalised();

            if (p1.z() < 0 && p2.z() > 0) continue;
            if (p1.z() > 0 && p2.z() < 0) continue;

            Vector3dd plane = (p1 ^ p2);
            double z = plane.z();
            cout << " " << i << " " << j << " " << plane << endl;
            if (z <= 0) {
                continue;
            }

            if (z < zMin) {
                zMin = z;
                mini = i;
                minj = j;
            }
        }
    }
    cout << "-> " << mini << " - " << minj << endl;

    Vector3dd prev    = points[mini];
    Vector3dd current = points[minj];


    if (debug) {
        debug->setColor(RGBColor::Yellow());
        debug->addLineDash(Vector3dd::Zero(), (prev ^ current) * 10.0);
    }

    output.push_back(prev);
    output.push_back(current);

    if (debug != NULL) {
        debug->setColor(RGBColor::Red());
        debug->addIcoSphere(prev.normalised(), 0.1);

        debug->setColor(RGBColor::Green());
        debug->addIcoSphere(current.normalised(), 0.1);
    }
#endif

    /* Wrap */
    int limit = 10;
    int iteration = 0;
    int mark = 0;
    do {
        iteration++;
        cout << "Iteration " << iteration << endl;
        //if (debug != NULL) debug->addLine(Vector3dd::Zero(), prev.normalised());

        Vector3dd next = Vector3dd::Zero();
        double vmax = std::numeric_limits<double>::lowest();

        /* Search for best point */
        for (const Vector3dd &point : points)
        {
            if ((point.normalised() - current.normalised()).l2Metric() < 1e-7) {
                continue;
            }

            Vector3dd splane = current ^ prev;
            if (iteration > mark) {
                //if (debug != NULL) debug->setColor(RGBColor::rainbow(lerp(0.0, 1.0, iteration, mark, limit)));
                if (debug != NULL) debug->setColor(RGBColor::Green() / 2);
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), prev.normalised(), 0.03, 0.03);

                if (debug != NULL) debug->setColor(RGBColor::Green());
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), current.normalised());


                if (debug != NULL) debug->setColor(RGBColor::Yellow());
                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), splane.normalised());

//                if (debug != NULL) debug->setColor(RGBColor::Pink());
//                if (debug != NULL) debug->addLineDash(Vector3dd::Zero(), back.normalised());
            }


            Vector3dd p = point.normalised();
            Vector3dd nplane = p ^ current;

            double v = splane.normalised() & nplane.normalised();

            /*if (v < 0) {
                return false;
            }*/
            /*
            bool forward = (p & back) < 0;
            bool part    = (p & current) > 0;
            */

            SYNC_PRINT(("(%lf %lf %lf) - %lf\n", point.x(), point.y(), point.z(), v));

            if (v > vmax /*&& forward*/ /*&& part*/)
            {
                vmax = v;
                next = point;
            }
        }

        if (next == output.front())
        {
            break;
        }
        SYNC_PRINT(("ConvexHull::GiftWrap(): %s next point (%lf %lf %lf)\n",
                    iteration > mark ? "*" : "",
                    next.x(), next.y(), next.z()));

        prev = current;
        current = next;
        output.push_back(next);
        //back = Vector3dd::OrtZ() ^ current;

        if (iteration > mark) {
            if (debug != NULL) debug->setColor(RGBColor::Navy());
            if (debug != NULL) debug->addLine(Vector3dd::Zero(), next.normalised());
        }

        //if (debug != NULL) debug->setColor(RGBColor::Magenta());


    } while (iteration < limit);
    cout << "Remaining polygons:" << output.size() << endl;
    return true;
}


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

    Vector2dd drawShift = Vector2dd( 100,  100);

    for (size_t i = 0; i < CORE_COUNT_OF(shifts); i++)
    {
        /* Move poligons to interesting positions  */
        Polygon pt1 = Polygon::Reversed(p1.translated(shifts[i]));
        Polygon pt2 = Polygon::Reversed(p2.translated(shifts[i]));

        ConvexPolygon cp1 = pt1.toConvexPolygon();
        ConvexPolygon cp2 = pt2.toConvexPolygon();

        Vector2dd check(50, 50);
        CORE_ASSERT_TRUE(cp1.isInside(check + shifts[i]), "Wrong orientation");
        CORE_ASSERT_TRUE(cp2.isInside(check + shifts[i]), "Wrong orientation");

      //ConvexPolygon cp3 = ConvexPolygon::merge(cp1, cp2);
#if 0
        ConvexPolygon cp3 = ConvexPolygon::merge(cp1, cp2);
        ProjectivePolygon pp = ProjectivePolygon::FromConvexPolygon(cp3);
        ProjectivePolygon pps;
        bool result = GiftWrap(pp, pps, NULL);
        cp3 = pps.toConvexPolygon();
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

        cout << "Shift:" << i << "  " << shifts[i] << endl;
        // cout << "pt1" << pt1 << endl;
        // cout << "pt2" << pt2 << endl;

        //cout << "result" << p3 << endl;

        drawPolygon2Color(&buffer, pt1.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
        drawPolygon2Color(&buffer, pt2.translated(drawShift), RGBColor::Green(), RGBColor::Yellow());
        drawPolygon2Color(&buffer, p3 .translated(drawShift), RGBColor::White(), RGBColor::Cyan());

        Polygon bp3 = p3.translated(-shifts[i]);
        cout << "result" << bp3 << endl;
#if 1
        CORE_ASSERT_TRUE(bp3.size() == 4, "Wrong intersect size");

        CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(99, 60    ), 0.1), "Wrong intersect point 0");
        CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(55, 79.775), 0.1), "Wrong intersect point 1");
        CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(11, 60    ), 0.1), "Wrong intersect point 2");
        CORE_ASSERT_TRUE(bp3.hasPoint(Vector2dd(55, 35.280), 0.1), "Wrong intersect point 3");
#endif
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(buffer, "non_eigen1.bmp");

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

    p1.transform(Matrix33::ShiftProj(10, -10) * Matrix33::ScaleProj(1/10.0));
    p2.transform(Matrix33::ShiftProj(10, -10) * Matrix33::ScaleProj(1/10.0));

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
    bool result = GiftWrap(points, simplified, &mesh);
    //bool result = ConvexHull::GiftWrap(points, simplified, &mesh);


    cout << "Result: " << (result ? "true" : "false") << endl;

#if 1
    ConvexPolygon cp4 = simplified.toConvexPolygon();
    mesh.setColor(RGBColor::Green());
    for (Line2d &line : cp4.faces)
    {
        // mesh.addPoint(Vector3dd(line.toDualS(), 0.6));
        mesh.addLineDash(Vector3dd::Zero(), line.toDualP().normalised(), 0.05, 0.1);

        double t1 = 0.0;
        double t2 = 0.0;
        Ray2d r = line.toRay();
        r.normalise();
        Circle2d circle2d(holder.center(), holder.size.l2Metric());
        if (circle2d.intersectWith(r, t1, t2))
        {
            mesh.addLine(
                Vector3dd(r.getPoint(t1), 1.0),
                Vector3dd(r.getPoint(t2), 1.0));

            for (double t = t1; t < t2; t+=1)
            {

                mesh.addLine(
                    Vector3dd(r.getPoint(t), 1.0),
                    Vector3dd(r.getPoint(t) + line.normal().normalised() * 0.3, 1.0));
            }
        }
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
    bool result = GiftWrap(points, simplified, &mesh);

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
