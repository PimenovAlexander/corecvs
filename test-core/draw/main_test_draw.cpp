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

using namespace corecvs;


typedef AttributedTriangleSpanIterator AttributedTriangleSpanIteratorFix;

TEST(Draw, testHistogram)
{
    RGB24Buffer *buffer = new RGB24Buffer(512, 512);

    Histogram hist(0, 255);
    for (size_t p = 0; p < hist.data.size(); p++) {
        hist.data[p] = sin(p / 100.0) * 255.0;
    }

    buffer->drawHistogram1024x512(&hist,   0,   0, 0x1, 256, 256);

    buffer->drawHistogram1024x512(&hist,   0, 256, 0x1, 256, 128);
    buffer->drawHistogram1024x512(&hist, 256,   0, 0x1, 128, 256);



    BMPLoader().save("hists.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, testBitBlt)
{
    RGB24Buffer in (48,48, RGBColor::Yellow());
    RGB24Buffer out(100,100, RGBColor::Green());

    out.fillWith(in, 1, 1);
    out.fillWith(in, 1, 51);
    out.fillWith(in, 51, 1);
    out.fillWith(in, 51, 51);

    BMPLoader().save("bitBlt.bmp", &out);
}


TEST(Draw, testCircles)
{
    RGB24Buffer *buffer = new RGB24Buffer(21, 52);

    RGBColor colors[] = {
        RGBColor::Red(),
        RGBColor::Blue(),
        RGBColor::Yellow(),
        RGBColor::Black(),
        RGBColor::Indigo()
    };

    for (int i = 11; i >=1; i-= 1)
    {
        AbstractPainter<RGB24Buffer>(buffer).drawCircle(10,10, i, colors[i % CORE_COUNT_OF(colors)] );
    }

    for (int i = 11; i >=1; i-= 2)
    {
        buffer->drawArc(33, 10, i, colors[i % CORE_COUNT_OF(colors)] );
    }

    BMPLoader().save("circles.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, testCircles1)
{
    RGB24Buffer *buffer = new RGB24Buffer(100, 100);

    buffer->drawArc(50, 50, 40, RGBColor::White() );

    for (int i = 0; i < 40; i++ )
    {
        Vector2dd point = Vector2dd(50.0, 50.0) + Vector2dd::FromPolar(degToRad(360.0 / 40 * i), 40);
        buffer->setElement(fround(point.y()), fround(point.x()), RGBColor::Blue());
    }

    BMPLoader().save("circles1.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, testCircleIterator)
{
    RGB24Buffer *buffer = new RGB24Buffer(100, 100);


    CircleSpanIterator inner(Circle2d(50, 50, 30));
    while (inner.hasValue())
    {
        HLineSpanInt span = inner.getSpan();
        while (span.hasValue()) {
            if (buffer->isValidCoord(span.pos())) {
                buffer->element(span.pos()) = RGBColor::Red();
            }
            span.step();
        };
        inner.step();
    }


    BMPLoader().save("circles-it.bmp", buffer);
    delete_safe(buffer);
}


TEST(Draw, testLineterator)
{
    RGB24Buffer *buffer  = new RGB24Buffer(100, 100);
    RGB24Buffer *buffer1 = new RGB24Buffer(100, 100);

    Segment<Vector2d32> segments[4] =
    {
        {{15, 10}, {50, 95}},
        {{10, 15}, {95, 50}},
        {{40, 85}, { 5,  5}},
        {{85, 40}, { 5,  5}}
    };

    RGBColor colors[4] = {
        RGBColor::Red(),
        RGBColor::Green(),
        RGBColor::Blue(),
        RGBColor::Cyan()
    };

    for (size_t i = 0; i <  CORE_COUNT_OF(segments); i++)
    {
        LineSpanIterator iterator(segments[i]);
        for (auto point : iterator)
        {
            buffer->element(point) = colors[i];
        }

        Vector3dd color1 = colors[i].toDouble();
        Vector3dd color2 = colors[(i + 1) % CORE_COUNT_OF(segments)].toDouble();

        FragmentAttributes c1(color1.element, color1.element + 3);
        FragmentAttributes c2(color2.element, color2.element + 3);

        cout << "Color1:" << color1 << endl;
        cout << "Color2:" << color2 << endl;

        cout << "Attr1:" << c1[0] << " " << c1[1] << " " << c1[2] << " "  << endl;
        cout << "Attr2:" << c2[0] << " " << c2[1] << " " << c2[2] << " " << endl;

        AttributedLineSpanIterator attIterator(segments[i].a.x(), segments[i].a.y(), segments[i].b.x(), segments[i].b.y(), c1, c2);
        while (attIterator.hasValue())
        {
            Vector2d<int> point = attIterator.pos();
            RGBColor color = RGBColor(attIterator.att()[0], attIterator.att()[1], attIterator.att()[2]);
            SYNC_PRINT(("We are at [%d %d] (%d %d %d)\n", point.x(), point.y(), color.r(), color.g(), color.b()));
            buffer1->element(point) = color;
            attIterator.step();
        }
    }

    BMPLoader().save("segments-it.bmp" , buffer);
    BMPLoader().save("segments1-it.bmp", buffer1);

    delete_safe(buffer);
    delete_safe(buffer1);

}


TEST(Draw, testRectangles)
{
    int h = 100;
    int w = 100;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    for (int i = 1; i < 8; i++) {
        int pos = (i* (i - 1)) / 2 + 1;

        buffer->drawRectangle( pos,  pos, i, i, RGBColor::Red()  , 0);

        buffer->drawRectangle(50 + pos,      pos, i, i, RGBColor::Green(), 1);
        buffer->drawRectangle(     pos, 50 + pos, i, i, RGBColor::Blue() , 2);
    }

    BMPLoader().save("rects.bmp", buffer);
    delete_safe(buffer);
}




TEST(Draw, testSpanDraw)
{
    int h = 200;
    int w = 300;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    {
        TrapezoidSpanIterator it(10, 40, 10, 30, 40, 90);
        while (it.hasValue())
        {
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Red());
            it.step();
        }
    }

    {
        TrapezoidSpanIterator it(110, 170, 10, 30, 5, 35);
        while (it.hasValue())
        {
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());
            it.step();
        }
    }

    {


        Vector2dd p1(210, 10);
        Vector2dd p2(205, 12);
        Vector2dd p3(285, 12);


        TrapezoidSpanIterator it(p1.y(), p2.y(), p1.x(), p1.x(), p2.x(), p3.x());
        while (it.hasValue())
        {
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());
            it.step();
        }
        buffer->drawPixel(p1.x(), p1.y(), RGBColor::Red());
        buffer->drawPixel(p2.x(), p2.y(), RGBColor::Red());
        buffer->drawPixel(p3.x(), p3.y(), RGBColor::Red());

    }

    {


        Vector2dd p1(216.381, 132.5448);
        Vector2dd p2(214.665, 109.82056);
        Vector2dd p3(282.487, 112.7939);

        AttributedTriangle triF = AttributedTriangle(p1, p2, p3);
        AttributedTriangleSpanIteratorFix itF(triF);
        while (itF.hasValue()  && itF.part.y1 < 111 )
        {
            AttributedHLineSpan span = itF.getAttrSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Blue());
            itF.step();
        }

        TrapezoidSpanIterator it(p1.y(), p2.y(), p1.x(), p1.x(), p2.x(), p3.x());
        while (it.hasValue())
        {
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());
            it.step();
        }
        buffer->drawPixel(p1.x(), p1.y(), RGBColor::Red());
        buffer->drawPixel(p2.x(), p2.y(), RGBColor::Red());
        buffer->drawPixel(p3.x(), p3.y(), RGBColor::Red());

    }

    {
        Triangle2dd t(Vector2dd(120, 30), Vector2dd(170, 80), Vector2dd(140, 10));
        TriangleSpanIterator it(t);
        while (it.hasValue())
        {
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Pink());
            it.step();
        }
    }

    {
        Triangle2dd t(Vector2dd(120, 130), Vector2dd(170, 180), Vector2dd(140, 110));
        TrianglePointIterator it(t);
        for (auto p: it) {
            buffer->element(p) = RGBColor::Violet();
        }
    }

    BMPLoader().save("spandraw.bmp", buffer);


    {
        TrapezoidSpanIterator it(10, 40, 10, 30, 40, 90);
        for(HLineSpanInt span: it) {
              buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Pink());
        }

    }

    BMPLoader().save("spandraw1.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, testSpanDrawTriangle)
{
    int h = 200;
    int w = 200;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    Triangle2dd t[] = {
        Triangle2dd(Vector2dd(10, 10), Vector2dd( 70, 10), Vector2dd(10, 90)),
        Triangle2dd(Vector2dd(70, 90), Vector2dd( 70, 10), Vector2dd(10, 90)),

        Triangle2dd(Vector2dd(110, 10), Vector2dd( 170, 10), Vector2dd(110, 90)),
        Triangle2dd(Vector2dd(171, 90), Vector2dd( 171, 10), Vector2dd(111, 90))
    };

    RGBColor c[] = {RGBColor::Pink(), RGBColor::Cyan(), RGBColor::Pink(), RGBColor::Cyan()};

    for (size_t i = 0; i < CORE_COUNT_OF(t); i++) {

        TriangleSpanIterator it(t[i]);
        while (it.hasValue())
        {           
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, c[i]);
            it.step();
        }
    }

    BMPLoader().save("triangledraw.bmp", buffer);
    delete_safe(buffer);

}

TEST(Draw, testSpanDrawTriangle1)
{
    int h = 2000;
    int w = 2000;
    int sample = 40;

    std::mt19937 rng;
    std::uniform_real_distribution<double> runif(2, sample - 2);

    RGB24Buffer *buffer  = new RGB24Buffer(h, w, RGBColor::Black());
    RGB24Buffer *bufferA = new RGB24Buffer(h, w, RGBColor::Black());
    RGB24Buffer *bufferF = new RGB24Buffer(h, w, RGBColor::Black());


    vector<Triangle2dd> t;
    for (int i = 0; i < (h / sample); i++)
        for (int j = 0; j < (w / sample); j++)
        {
            Triangle2dd tri(Vector2dd(runif(rng), runif(rng)), Vector2dd(runif(rng), runif(rng)), Vector2dd(runif(rng), runif(rng)));
            tri.transform(Matrix33::ShiftProj(j * sample, i * sample));
            t.push_back(tri);
        }

    for (int i = 0; i < (h / sample); i++)
        for (int j = 0; j < (w / sample); j++)
        {
            int offset = i  * (w / sample) + j;
            AbstractPainter<RGB24Buffer> p(buffer);
            p.drawFormat(j * sample, i * sample, RGBColor::Blue(), 2, "%d", offset);
            AbstractPainter<RGB24Buffer> pA(bufferA);
            pA.drawFormat(j * sample, i * sample, RGBColor::Blue(), 2, "%d", offset);
            AbstractPainter<RGB24Buffer> pF(bufferF);
            pF.drawFormat(j * sample, i * sample, RGBColor::Blue(), 2, "%d", offset);
            Triangle2dd &tri = t[offset];

            if (offset == 12)
                cout << tri << endl;


            {
                TriangleSpanIterator it(tri);
                while (it.hasValue())
                {
                    HLineSpanInt span = it.getSpan();
                    buffer->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());

                    if (offset == 12)
                        cout << span << endl;


                    it.step();
                }
            }
            /**/
            {
                AttributedTriangle triA = AttributedTriangle(tri.p1(), tri.p2(), tri.p3());
                AttributedTriangleSpanIterator itA(triA);
                while (itA.hasValue())
                {
                    AttributedHLineSpan span = itA.getAttrSpan();
                    bufferA->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());
                    itA.step();
                }
            }
            /**/
            {


                AttributedTriangle triF = AttributedTriangle(tri.p1(), tri.p2(), tri.p3());
                AttributedTriangleSpanIteratorFix itF(triF);
                while (itF.hasValue())
                {
                    AttributedHLineSpan span = itF.getAttrSpan();
                    bufferF->drawHLine(span.x1, span.y(), span.x2, RGBColor::Green());
                    itF.step();
                }
            }

            /* Mark the corners */
            for (int k = 0; k < tri.SIZE; k++) {
                buffer ->element(fround(tri.p[k].y()), fround(tri.p[k].x())) =  RGBColor::Red();
                bufferA->element(fround(tri.p[k].y()), fround(tri.p[k].x())) =  RGBColor::Red();
                bufferF->element(fround(tri.p[k].y()), fround(tri.p[k].x())) =  RGBColor::Red();

            }



    }
    /**/




    BMPLoader().save("trianglemany.bmp", buffer);
    BMPLoader().save("triangleAmany.bmp", bufferA);
    BMPLoader().save("triangleFmany.bmp", bufferF);

    delete_safe(bufferF);
    delete_safe(bufferA);
    delete_safe(buffer);

}

TEST(Draw, testAttributedTriangle)
{
    int h = 300;
    int w = 300;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    AttributedTriangle t(Vector2dd(0,0), Vector2dd(0,300), Vector2dd(300,150));
    RGBColor c[3] = {RGBColor::Red(), RGBColor::Green(), RGBColor::Blue()};

    for (int i = 0; i < 3; i++) {
        t.p[i].attributes.push_back(c[i].r());
        t.p[i].attributes.push_back(c[i].g());
        t.p[i].attributes.push_back(c[i].b());
    }

    AttributedTriangleSpanIterator it(t);

    while (it.hasValue())
    {        
        AttributedHLineSpan span = it.getAttrSpan();
        while (span.hasValue())
        {
            if (buffer->isValidCoord(span.pos()) ) {
                Vector3dd color(span.att()[0], span.att()[1], span.att()[2]);
                buffer->element(span.pos()) = RGBColor::FromDouble(color);
            }
            span.step();
        }
        it.step();
    }

    BMPLoader().save("attributed.bmp", buffer);
}

TEST(Draw, testSpanRenderTriangle)
{
    int h = 300;
    int w = 300;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    Triangle3dd t[2] = {
        Triangle3dd(Vector3dd(-100,-100, 100), Vector3dd(-100, 100, 100), Vector3dd( 100, 0, 120)),
        Triangle3dd(Vector3dd( 100,-100, 100), Vector3dd( 100, 100, 100), Vector3dd(-100, 0, 120)),
    };

    Triangle2dd p[CORE_COUNT_OF(t)];
    for (size_t i = 0; i < CORE_COUNT_OF(t); i++)
    {
        p[i] = Triangle2dd(t[i].p1().xy() + Vector2dd(100.0, 100.0),
                           t[i].p2().xy() + Vector2dd(100.0, 100.0),
                           t[i].p3().xy() + Vector2dd(100.0, 100.0));
    }



    RGBColor c[CORE_COUNT_OF(t)] = {RGBColor::Pink(), RGBColor::Cyan()};

    for (size_t i = 0; i < CORE_COUNT_OF(t); i++) {

        TriangleSpanIterator it(p[i]);
        while (it.hasValue())
        {          
            HLineSpanInt span = it.getSpan();
            buffer->drawHLine(span.x1, span.y(), span.x2, c[i]);
            it.step();
        }
    }
    BMPLoader().save("renderdraw.bmp", buffer);


    /* Z buffer */
    AbstractBuffer<double> *zBuffer = new AbstractBuffer<double>(h,w, /*std::numeric_limits<double>::max()*/ 140.0);

    AttributedTriangle a[CORE_COUNT_OF(t)];
    for (size_t i = 0; i < CORE_COUNT_OF(t); i++)
    {
        a[i] = AttributedTriangle(t[i].p1().xy() + Vector2dd(100.0, 100.0),
                                  t[i].p2().xy() + Vector2dd(100.0, 100.0),
                                  t[i].p3().xy() + Vector2dd(100.0, 100.0));
        a[i].p1().attributes.push_back(t[i].p1().z());
        a[i].p2().attributes.push_back(t[i].p2().z());
        a[i].p3().attributes.push_back(t[i].p3().z());
    }


    for (size_t i = 0; i < CORE_COUNT_OF(t); i++) {

        AttributedTriangleSpanIterator it(a[i]);

        while (it.hasValue())
        {            
            HLineSpanInt span = it.getSpan();
            double z1 = it.part.a1[0];
            double z2 = it.part.a2[0];

            for (int j = span.x1; j <= span.x2; j++) {
                if (zBuffer->isValidCoord(span.y(), j) ) {
                    double z = lerp(z1,z2, j, span.x1, span.x2 + 1);
//                    SYNC_PRINT(("Z=%lf, WAS=%lf\n", z, zBuffer->element(span.y(), j)));

                    if (zBuffer->element(span.y(), j) >  z) {
                         zBuffer->element(span.y(), j) = z;
                         buffer->element(span.y(), j) = c[i];
                    }
                }
            }
            it.step();
        }
    }
    BMPLoader().save("renderdraw-rgb.bmp", buffer);





    buffer->drawDoubleBuffer(zBuffer);
    BMPLoader().save("renderdraw-z.bmp", buffer);
    delete_safe(buffer);
    delete_safe(zBuffer);

}

#if 0
TEST(Draw, testSpanDrawTriangleSpeed)
{
    //AttributedTriangle a[CORE_COUNT_OF(t)];
    //AttributedTriangleSpanIterator it(a[i]);
}
#endif

TEST(Draw, polygonDraw)
{
    int h = 300;
    int w = 300;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    Vector2dd center(w / 2.0, h / 2.0);

    AbstractPainter<RGB24Buffer> painter(buffer);

    const int SIZE = 4;
    const double PHASE = (M_PI / 6);
    corecvs::Polygon p;
    for (int i = 0; i < SIZE; i++) {
        p.push_back(Vector2dd::FromPolar(-(2 * M_PI / SIZE) * i + PHASE, 100.0) + center);
    }

    cout << "Poligon" << std::endl << p << std::endl;
    cout << "Area1" << p.signedArea() << endl;

    painter.drawPolygon(p, RGBColor::Blue());

    PolygonPointIterator it(p);
    for (Vector2d<int> p: it) {
        if (buffer->isValidCoord(p)) {
            buffer->element(p) += RGBColor(140, 140, 0);
        }
    }

    BMPLoader().save("polygon.bmp", buffer);
    delete_safe(buffer);
}


TEST(Draw, polygonDraw1)
{
    int h = 240;
    int w = 240;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    Vector2dd center(buffer->h / 12, buffer->h / 12);
    double radius = center.x() / 10.0 * 9.0;

    AbstractPainter<RGB24Buffer> painter(buffer);

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar((2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1));
            }

            cout << "Area1" << p.signedArea() << endl;
           // cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Blue());

            PolygonSpanIterator it(p);
            for (HLineSpanInt l: it)
            {
                for (Vector2d<int> point : l) {
                    if (buffer->isValidCoord(point)) {
                        buffer->element(point) += RGBColor(90, 90, 0);
                    }
                }
            }           
        }
    }

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar(-(2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1) + Vector2dd(0.0, buffer->h / 2));
            }

            cout << "Area1" << p.signedArea() << endl;
           // cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Green());

            PolygonSpanIterator it(p);
            for (HLineSpanInt l: it)
            {
//                cout << l << endl;
                for (Vector2d<int> point : l) {
                    if (buffer->isValidCoord(point)) {
                        buffer->element(point) += RGBColor(0, 70, 0);
                    }
                }
            }
        }
    }

    for (int pi = 0; pi < 3; pi ++)
    {
        for (int pj = 0; pj < 3; pj ++)
        {
            corecvs::Polygon p;
            int count = pi * 3 + pj + 3;
            for (int i = 0; i < count; i++) {
                p.push_back(Vector2dd::FromPolar(-(2 * M_PI / count) * i, radius) + center * Vector2dd(pi * 2 + 1, pj * 2 + 1) + Vector2dd(buffer->w / 2, 0.0));
            }

            cout << "Area1" << p.signedArea() << endl;
           //cout << p << std::endl;

            painter.drawPolygon(p, RGBColor::Green());

            PolygonPointIterator it(p);
            for (Vector2d<int> point : it) {
                    buffer->element(point) += RGBColor(0, 70, 0);
            }
        }
    }


    BMPLoader().save("polygon1.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, polygonDraw2)
{
    int h = 240;
    int w = 240;

    corecvs::Polygon p;
    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());

    p.push_back(Vector2dd(57.439, 16.983));
    p.push_back(Vector2dd(45.324, 28.776));
    p.push_back(Vector2dd(37.829, 16.898));
    p.push_back(Vector2dd(48.848, 05.05));

    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawPolygon(p, RGBColor::Blue());
    BMPLoader().save("polygon2.bmp", buffer);

    PolygonPointIterator it(p);
    for (Vector2d<int> point : it) {
        //cout << point << endl;
        ASSERT_TRUE(buffer->isValidCoord(point));
        buffer->element(point) += RGBColor::Red();
    }

    BMPLoader().save("polygon2.bmp", buffer);
    delete_safe(buffer);
}

TEST(Draw, testFloodFill)
{
    int h = 20;
    int w = 20;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);
    painter.drawCircle(    w / 4,     h / 4, w / 5, RGBColor::Red() );
    painter.drawCircle(    w / 4, 3 * h / 4, w / 5, RGBColor::Red() );
    painter.drawCircle(3 * w / 4,     h / 4, w / 5, RGBColor::Red() );
    painter.drawCircle(3 * w / 4, 3 * h / 4, w / 5, RGBColor::Red() );

    BMPLoader().save("flood_before.bmp", buffer);

    AbstractPainter<RGB24Buffer>::EqualPredicate predicate(RGBColor::Black(), RGBColor::Blue());
    painter.floodFill(w / 2.0, h / 2.0, predicate);

    BMPLoader().save("flood_after.bmp", buffer);
    //printf("Predicate  : %d\n", predicate.countPred);
    //printf("Mark       : %d\n", predicate.countMark);
    //printf("Double Mark: %d\n", predicate.doubleMark);

    delete_safe(buffer);
}

TEST(Draw, testRobot)
{
    int h = 300;
    int w = 300;

    RGB24Buffer *buffer = new RGB24Buffer(h, w, RGBColor::Black());
    AbstractPainter<RGB24Buffer> painter(buffer);

    double r = 20;
    Circle2d center  (150,150, r);
    Circle2d perifery[4] =
    {
        Circle2d(100, 110, r),
        Circle2d(200, 110, r),

        Circle2d(110, 190, r),
        Circle2d(190, 190, r)
    };

    double l1 = 50*1.4 - r;
    double l2 = 50*1.4 + r ;



    painter.drawCircle(center, RGBColor::Blue());

    painter.drawCircle( perifery[0], RGBColor::Yellow());
    painter.drawCircle( perifery[2], RGBColor::Yellow());

    painter.drawCircle( perifery[1], RGBColor::Red());
    painter.drawCircle( perifery[3], RGBColor::Red());

    BMPLoader().save("robot.bmp", buffer);

    AbstractBuffer<double> acc(buffer->getSize());

   /** */

    CircleSpanIterator outer(center);
    while (outer.hasValue())
    {
        HLineSpanInt span = outer.getSpan();
        for (int s = span.x1; s < span.x2; s++ )
        {
            CircleSpanIterator inner(Circle2d(s, span.y(), r/2));
            while (inner.hasValue())
            {
                HLineSpanInt span = inner.getSpan();
                for (int s1 = span.x1; s1 < span.x2; s1++ )
                {
                    if (acc.isValidCoord(span.y(), s1))
                    {
                        acc.element(span.y(), s1)++;
                    }
                }
                inner.step();
            }
        }
        outer.step();
    }

    /**/
    for (int c = 0; c < 4; c++)
    {
        CircleSpanIterator outer(perifery[c]);
        while (outer.hasValue())
        {
            HLineSpanInt span = outer.getSpan();
            for (int s = span.x1; s < span.x2; s++ )
            {
                CircleSpanIterator inner(Circle2d(s, span.y(), l2));
                while (inner.hasValue())
                {
                    HLineSpanInt span = inner.getSpan();
                    for (int s1 = span.x1; s1 < span.x2; s1++ )
                    {
                        //SYNC_PRINT(("#"));
                        if (acc.isValidCoord(span.y(), s1))
                        {
                            acc.element(span.y(), s1)++;
                        }
                    }
                    inner.step();
                }

                CircleSpanIterator inner1(Circle2d(s, span.y(), l1));
                while (inner1.hasValue())
                {
                    HLineSpanInt span = inner1.getSpan();
                    for (int s1 = span.x1; s1 < span.x2; s1++ )
                    {
                        //SYNC_PRINT(("#"));
                        if (acc.isValidCoord(span.y(), s1))
                        {
                            acc.element(span.y(), s1)--;
                        }
                    }
                    inner1.step();
                }
            }
            outer.step();
        }

    }


    buffer->drawDoubleBuffer(acc);
    BMPLoader().save("robot2.bmp", buffer);

    delete_safe(buffer);
}

TEST(Draw, DISABLED_testPack)
{
    RGB24Buffer *buffer = BufferFactory::getInstance()->loadRGB24Bitmap("input.bmp");
    if (buffer == NULL)
        return;

    SYNC_PRINT(("Loaded [%d %d]\n", buffer->h, buffer->w));

    RGB24Buffer *block = new RGB24Buffer(48,48);
    block->fillWith(*buffer);

    AbstractPainter<RGB24Buffer> painter(block);
    block->fillWith(RGBColor::White());

    //painter.drawCircle(24, 24, 10, RGBColor::Blue());
    //painter.drawCircle(24, 24,  4, RGBColor::Red());
    //painter.drawHLine(0, 0, 47, RGBColor::Blue());
    painter.drawFormat(0, 10, RGBColor::Yellow(), 1, "Test");

    BMPLoader().save("test.bmp", block);

    #define BLOCK_NUM  6

    uint8_t lines[6][16 * BLOCK_NUM * 8][2];
    for (int lineid = 0; lineid < 6; lineid++)
    {
        for (int pixel = 0; pixel < 16; pixel++) /* over pixels */
        {
            for (int blockn = 0; blockn < BLOCK_NUM; blockn++) /* over blocks */
            {
                uint32_t x  =  0;
                if (pixel & 0x8) {
                  x = (pixel & 0x07) + (blockn * 8);
                } else {
                  x = (blockn * 8) + 7 - (pixel & 0x07);
                }

                uint32_t y1 =  lineid + ((pixel & 0x08) ?  0 : 6);


                uint32_t  px3 = block->element(y1     , x).toBGRInt();
                uint32_t  px4 = block->element(y1 + 12, x).toBGRInt();
                uint32_t  px1 = block->element(y1 + 24, x).toBGRInt();
                uint32_t  px2 = block->element(y1 + 36, x).toBGRInt();

                /*
                uint32_t  px1 = 0xFFFFFF;
                uint32_t  px2 = 0xFFFFFF;
                uint32_t  px3 = 0xFFFFFF;
                uint32_t  px4 = 0xFF00FF;
                */


               /* We have 4 colors now. */

                for (int bitn = 0; bitn < 8; bitn++) /* over bits */
                {
                        uint8_t c =  ((px1 & 0x800000) >> 18) | ((px1 & 0x8000) >> 11) | ((px1 & 0x80) >> 4)   |  ((px2 & 0x800000) >> 21) | ((px2 & 0x8000) >> 14) | ((px2 & 0x80) >> 7) ;
                        uint8_t b =  ((px3 & 0x800000) >> 18) | ((px3 & 0x8000) >> 11) | ((px3 & 0x80) >> 4)   |  ((px4 & 0x800000) >> 21) | ((px4 & 0x8000) >> 14) | ((px4 & 0x80) >> 7) ;
                        px1 <<= 1;
                        px2 <<= 1;
                        px3 <<= 1;
                        px4 <<= 1;

                        lines[lineid][pixel * BLOCK_NUM * 8 + blockn * 8 + bitn][0] = c;
                        lines[lineid][pixel * BLOCK_NUM * 8 + blockn * 8 + bitn][1] = b;
                }
            }
        }

    }


    std::ofstream os;
    os.open("out.c", std::ofstream::out);

    os << "#include <avr/pgmspace.h>\n";
    os << "\n";
    os << "const unsigned char packed[] PROGMEM = {\n";

    for (int lineid = 0; lineid < 6; lineid++)
    {
        for (int data = 0; data < 16 * BLOCK_NUM * 8; data++)
        {
            if ((data % 16) == 0) {
                 os << "\n";
            }

            os << (( lineid == 0 && data == 0 )? "  " : ", ") <<  (uint)lines[lineid][data][0] << ", " << (uint)lines[lineid][data][1] ;

        }
        os << "/* */\n";
    }

    os << "};\n";

    os.close();

    delete_safe(buffer);
    delete_safe(block);

}
