/**
 * \file main_test_wuRasterizer.cpp
 * \brief This is the main file for the test wuRasterizer 
 *
 * \date Jan 02, 2018
 * \author vschellb
 *
 * \ingroup autotest  
 */

#include <iostream>
#include "gtest/gtest.h"

#include "core/utils/global.h"
#include "core/buffers/rgb24/wuRasterizer.h"
#include "core/buffers/abstractBuffer.h"
#include "core/buffers/rgb24/rgb24Buffer.h"
#include "core/buffers/rgb24/abstractPainter.h"
#include "core/buffers/bufferFactory.h"


using namespace corecvs;

template<typename T>
class TracerBuffer {
public:
    typedef T InternalElementType;

    T dummy;
    T & element(int y, int x) {
        SYNC_PRINT(("Access to element x,y = [%d x %d]\n", x, y));
        return dummy;
    }

    int getH() {return 100;}
    int getW() {return 100;}

};

TEST(wuRasterizer, testwuRasterizer) {
    AbstractBuffer<float> buffer1(100, 100, true);
    WuRasterizer::drawLine(buffer1, 0, 10, 10, 10, 2.);
    ASSERT_EQ(buffer1.element(0,10), 1.0);
    ASSERT_EQ(buffer1.element(0,11), 0.0);
    ASSERT_EQ(buffer1.element(10,10), 1.0);
    ASSERT_EQ(buffer1.element(10,11), 0.0);

    AbstractBuffer<float> buffer2(100, 100, true);
    WuRasterizer::drawLine(buffer2, 0, 0, 10, 10, 2.);
    ASSERT_EQ(buffer2.element(0,0), 1.0);
    ASSERT_EQ(buffer2.element(0,1), 0.0);
    ASSERT_EQ(buffer2.element(10,9), 0.0);
    ASSERT_EQ(buffer2.element(10,10), 1.0);
    ASSERT_EQ(buffer2.element(10,11), 0.0);


    TracerBuffer<RGBColor> trace;
    SYNC_PRINT(("drawLine(trace, 0, 0, 10, 10, 2.):\n"));
    WuRasterizer::drawLine(trace, 0, 0, 10, 10, RGBColor::Red());

    SYNC_PRINT(("drawLine(trace, 0, 0, 10, 5, 2.):\n"));
    WuRasterizer::drawLine(trace, 61, 50, 83, 50, RGBColor::Red());
}

TEST(wuRasterizer, testwuRasterizerGraphics) {
    const static int H = 100;
    const static int W = 100;

    const static int N = 8;


    RGB24Buffer buffer(H, W * 2);
    buffer.checkerBoard(10, RGBColor::White(), RGBColor::Gray(50));

    double c = 0;
    for (int i = 0; i < N; i++, c+= (1.0 / N))
    {

        double a = degToRad(c * 360.0);
        double dx = (cos(a) * H) / 3;
        double dy = (-sin(a) * H) / 3;

        int x1 = W / 2 + dx / 3;
        int y1 = H / 2 + dy / 3;
        int x2 = W / 2 + dx;
        int y2 = H / 2 + dy;


        buffer.drawLineSimpleFancy(    x1 , y1,     x2, y2, RGBColor::rainbow(c));
        buffer.drawLineSimple     (W + x1 , y1, W + x2, y2, RGBColor::rainbow(c));

        AbstractPainter<RGB24Buffer> p(&buffer);
        p.drawFormat(W + x2 + 3, y2, RGBColor::rainbow(c), 1, "%d", i);

        SYNC_PRINT(("%d call [%d %d]  -> [%d %d]\n", i, x1, y1, x2, y2));
    }

    BufferFactory::getInstance()->saveRGB24Bitmap(&buffer, "line.bmp");
}
