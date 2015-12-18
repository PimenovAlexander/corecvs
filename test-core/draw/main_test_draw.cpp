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
#include "gtest/gtest.h"

#include "global.h"

#include "rgb24Buffer.h"
#include "bmpLoader.h"
#include "abstractPainter.h"


using namespace std;
using namespace corecvs;

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
    painter.floodFill(w / 2, h / 2, predicate);

    BMPLoader().save("flood_after.bmp", buffer);
    //printf("Predicate  : %d\n", predicate.countPred);
    //printf("Mark       : %d\n", predicate.countMark);
    //printf("Double Mark: %d\n", predicate.doubleMark);

    delete_safe(buffer);
}
